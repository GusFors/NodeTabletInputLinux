#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <linux/input.h>
#include <nan.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include "display.h"

int32_t fd;
int32_t fdn;

void initUinputN(std::string name, int32_t xMax, int32_t yMax) {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    printf("error opening uinput");
    exit(EXIT_FAILURE);
  }
  // int widthtest = get_displays_total_width();
  // std::cout << widthtest << "widthtest\n";

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_POINTER);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_EVBIT, EV_ABS);
  // ioctl(fd, UI_SET_ABSBIT, ABS_X);
  // ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  std::string devName = "Virtual uinput " + std::string(name);

  struct uinput_setup uiPointer;
  memset(&uiPointer, 0, sizeof(uiPointer));
  snprintf(uiPointer.name, UINPUT_MAX_NAME_SIZE, "%s", devName.c_str());

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
  // abs_yprops.absinfo = {.value = 0, .minimum = 0, .maximum = yMax, .fuzz = 0, .flat = 0, .resolution = 0};
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  // write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);
}

bool isClick = false;
int32_t clickValue = 0;
int32_t mBtn = 0;
bool isActive = false;
int32_t xOffset;
int32_t yOffset;
int32_t yPrimaryHeight;
int32_t xPrimaryWidth;

void setUinputPointerN(int32_t x, int32_t y, int32_t pressure, int32_t btn) {

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

  // std::cout << ((btn & 0xff) & 0x07);
  // printf("%08b\n", btn & 0b00000111);

  switch (((btn & 0xff) & 0x07)) {
  case 0x01:
    clickValue = 1;
    if (isClick == false) {
      mBtn = BTN_LEFT;
      // std::cout << mBtn << "down\n";
      isClick = true;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
    }
    break;

  case 0x04:
    clickValue = 1;
    if (isClick == false) {
      mBtn = BTN_RIGHT;
      // std::cout << mBtn << "down\n";
      isClick = true;
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
      isClick = false;
      // std::cout << mBtn << " up\n";
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

void print_hex_buffer(int8_t *buf) {
  int32_t res;
  res = read(fdn, buf, 16);

  int32_t x = 0;
  if (res < 0) {
    perror("read err");
  } else {
    // x = buf[2] | (buf[3] << 8);
    // for (i = 0; i < res; i++) {
    //   //   printf("%hhx ", buf[i]);
    //   printf("%02hhx ", buf[i]);
    // }
    printf("\n");
    // printf("%d ", (buf[1] & (2 ^ 7)) );
    // printf("%08x ", (buf[1] & 0b00000001));
    for (int i = 0; i < 8; i++) {
      printf("%02hhx ", buf[i]);
      // int32_t bit = 2 ^ i;
      // printf("%x ", (buf[1] & bit) );
      //   printf("%d ", (buf[1] & (2 ^ i)) );
    }
  }
};

NAN_METHOD(initRead) {
  bool running = false;
  char *device;

  device = (*Nan::Utf8String(info[0]));
  std::cout << "Created uinput device: " << *Nan::Utf8String(info[1]) << "\n";

  fdn = open(device, O_RDONLY | O_SYNC);

  if (fdn < 0) {
    printf("Unable to open device");
    exit(EXIT_FAILURE);
  }
  printf("reading reports from: %s", device);

  // struct hidraw_report_descriptor rd;
  // struct hidraw_devinfo hinf;
  // memset(&rd, 0x0, sizeof(rd));
  // memset(&hinf, 0x0, sizeof(hinf));

  initUinputN(*Nan::Utf8String(info[1]), Nan::To<int32_t>(info[2]).FromJust(), Nan::To<int32_t>(info[3]).FromJust());
  running = true;

  // TODO: read from json config file instead of going through node to send all values
  int32_t left = Nan::To<int32_t>(info[4]).FromJust();
  int32_t top = Nan::To<int32_t>(info[5]).FromJust();
  double xScale = Nan::To<double>(info[6]).FromJust();
  double yScale = Nan::To<double>(info[7]).FromJust();

  xOffset = get_primary_monitor_xoffset();
  yOffset = get_primary_monitor_yoffset();
  xPrimaryWidth = get_primary_monitor_width();
  yPrimaryHeight = get_primary_monitor_height();
  int v = close_display();

  // xOffset = Nan::To<int32_t>(info[8]).FromJust();
  // yOffset = Nan::To<int32_t>(info[9]).FromJust();
  // xPrimaryWidth = Nan::To<int32_t>(info[10]).FromJust();
  // yPrimaryHeight = Nan::To<int32_t>(info[11]).FromJust();

  int32_t xBufferPos = Nan::To<int32_t>(info[12]).FromJust();
  int32_t yBufferPos = Nan::To<int32_t>(info[13]).FromJust();

  std::cout << "\n"
            << "xOffset: " << xOffset << " yOffset: " << yOffset;
  std::cout << "\n"
            << "xPrimaryWidth: " << xPrimaryWidth << " yPrimaryHeight: " << yPrimaryHeight;
  std::cout << "\n"
            << "xBufferPos: " << xBufferPos << " yBufferPos: " << yBufferPos;

  int32_t x = 0;
  int32_t y = 0;
  double xS = 0;
  double yS = 0;
  int32_t res;

  int8_t buf[256];
  memset(buf, 0x0, sizeof(buf));

  while (running) {
    res = read(fdn, buf, 16);

    if (res < 0) {
      perror("read err");
      exit(EXIT_FAILURE);
    } else {
      // print_hex_buffer(buf);

      x = (buf[xBufferPos] & 0xff) | ((buf[xBufferPos + 1] & 0xff) << 8);
      y = (buf[yBufferPos] & 0xff) | ((buf[yBufferPos + 1] & 0xff) << 8);

      // x = (buf[2] & 0xff) | ((buf[3] & 0xff) << 8);
      // y = (buf[4] & 0xff) | ((buf[5] & 0xff) << 8);

      xS = (x - left) * xScale;
      yS = (y - top) * yScale;

      if (xS < 0)
        xS = 0;

      if (yS < 0)
        yS = 0;

      if ((buf[0] & 0xff) < 0x11) {
        // std::cout << ((buf[1] & 0xff) & 0x07) << "\n";
        setUinputPointerN(xS, yS, 0, buf[1]);
      }

      // if ((buf[0] & 0xff) < 0x10) {
      //   // std::cout << ((buf[1] & 0xff) & 0x07) << "\n";
      //   setUinputPointerN(xS, yS, 0, buf[1]);
      // }
    }
  }
}

NAN_MODULE_INIT(init) { Nan::SetMethod(target, "initRead", initRead); }

NODE_MODULE(pointerN, init);