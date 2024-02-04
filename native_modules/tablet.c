#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include "display.h"
#include "tablet.h"
#include <fcntl.h>
#include <unistd.h>

int32_t fd;
int32_t fdn;

void init_uinput(const char *name, int32_t xMax, int32_t yMax) {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    printf("error opening uinput\n");
    perror("uinput open err");
    exit(EXIT_FAILURE);
  }

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_POINTER);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_EVBIT, EV_ABS);

  // ioctl(fd, UI_SET_ABSBIT, ABS_X);
  // ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  struct uinput_setup uiPointer;
  memset(&uiPointer, 0, sizeof(uiPointer));
  snprintf(uiPointer.name, UINPUT_MAX_NAME_SIZE, "%s", name);

  uiPointer.id.bustype = BUS_USB;
  uiPointer.id.version = 0;
  uiPointer.id.vendor = 0x0;
  uiPointer.id.product = 0x0;
  ioctl(fd, UI_DEV_SETUP, &uiPointer);

  struct uinput_abs_setup abs_xprops;
  abs_xprops.code = ABS_X;
  abs_xprops.absinfo.minimum = 0;
  abs_xprops.absinfo.maximum = xMax;
  // abs_xprops.absinfo = (input_absinfo){.value = 0, .minimum = 0, .maximum = xMax, .fuzz = 0, .flat = 0, .resolution =
  // 0};
  ioctl(fd, UI_ABS_SETUP, &abs_xprops);

  struct uinput_abs_setup abs_yprops;
  abs_yprops.code = ABS_Y;
  abs_yprops.absinfo.minimum = 0;
  abs_yprops.absinfo.maximum = yMax;
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  // write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);
}

int32_t isClick = 0;
int32_t clickValue = 0;
int32_t mBtn = 0;
int32_t isActive = 0;
int32_t xOffset;
int32_t yOffset;
int32_t yPrimaryHeight;
int32_t xPrimaryWidth;

void tablet_input_event(int32_t x, int32_t y, int32_t pressure, int32_t btn) {
  if (x > xPrimaryWidth)
    x = xPrimaryWidth;

  if (x < 0)
    x = 0;

  if (y < 0)
    y = 0;

  if (y > yPrimaryHeight)
    y = yPrimaryHeight;

  if (x == 0 && y == 0) {
    return;
  }

  // std::cout << "x: " << x << " y: " << y << "\n";

  struct input_event positionEvents[4];
  memset(&positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_TOOL_PEN;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  positionEvents[1].type = EV_ABS;
  positionEvents[1].code = ABS_X;
  positionEvents[1].value = x + xOffset;
  positionEvents[1].time.tv_sec = 0;
  positionEvents[1].time.tv_usec = 0;

  positionEvents[2].type = EV_ABS;
  positionEvents[2].code = ABS_Y;
  positionEvents[2].value = y + yOffset;
  positionEvents[2].time.tv_sec = 0;
  positionEvents[2].time.tv_usec = 0;

  // printf("%08b\n", btn & 0b00000111);

  switch (((btn & 0xff) & 0x07)) {
  case 0x01:
    clickValue = 1;
    if (isClick == 0) {
      mBtn = BTN_LEFT;
      isClick = 1;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
    }
    break;

  case 0x04:
    clickValue = 1;
    if (isClick == 0) {
      mBtn = BTN_RIGHT;
      isClick = 1;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
    }
    break;

  case 0x00:
    clickValue = 0;
    if (isClick) {
      isClick = 0;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
      mBtn = 0;
    }
  }

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = SYN_REPORT;

  write(fd, &syncEvent, sizeof(syncEvent));
}

void print_hex_buffer(int8_t *buf, int len) {
  printf("\n");
  for (int i = 0; i < len; i++) {
    printf("%02hhx ", buf[i]);
    // printf("%d ", (buf[1] & (2 ^ i)) );
  }
}

void parse_tablet_buffer(struct tablet_config tablet) {
  int32_t x = 0;
  int32_t y = 0;
  double xS = 0;
  double yS = 0;
  int32_t r;
  int32_t active = 1;

  int8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    r = read(fdn, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    } else {
      // print_hex_buffer(buf, r);

      x = (buf[tablet.xindex] & 0xff) | ((buf[tablet.xindex + 1] & 0xff) << 8);
      y = (buf[tablet.yindex] & 0xff) | ((buf[tablet.yindex + 1] & 0xff) << 8);

      xS = (x - tablet.left) * tablet.xscale;
      yS = (y - tablet.top) * tablet.yscale;

      if (xS < 0)
        xS = 0;

      if (yS < 0)
        yS = 0;

      if ((buf[0] & 0xff) < 0x11) {
        tablet_input_event(xS, yS, 0, buf[1]);
      }
    }
  }
}

void init_read(struct tablet_config tablet, struct display_config display_conf, const char *hidraw_path) {
  char tablet_path[32];
  strlcpy(tablet_path, hidraw_path, 32);

  xOffset = display_conf.xoffset;
  yOffset = display_conf.yoffset;
  xPrimaryWidth = display_conf.primary_width;
  yPrimaryHeight = display_conf.primary_height;

  fdn = open(tablet_path, O_RDONLY | O_SYNC);
  // fdn = open(tablet_path, O_RDONLY | O_NONBLOCK);

  if (fdn < 0) {
    printf("Unable to open device with path %s:", tablet_path);
    perror("\nread err");
    exit(EXIT_FAILURE);
  }

  printf("reading reports from: %s", tablet_path);

  parse_tablet_buffer(tablet);
}
