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

int fd;
int fdn;

void init_uinput(const char *name, int x_max, int y_max) {
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
  abs_xprops.absinfo.maximum = x_max;
  // abs_xprops.absinfo = (input_absinfo){.value = 0, .minimum = 0, .maximum = x_max, .fuzz = 0, .flat = 0, .resolution =
  // 0};
  ioctl(fd, UI_ABS_SETUP, &abs_xprops);

  struct uinput_abs_setup abs_yprops;
  abs_yprops.code = ABS_Y;
  abs_yprops.absinfo.minimum = 0;
  abs_yprops.absinfo.maximum = y_max;
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  // write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);
}

int is_click = 0;
int click_value = 0;
int mbtn = 0;
int offset_x;
int offset_y;
int primary_height;
int primary_width;

void create_input(int ev_type, int ev_code, int ev_value, struct input_event *ev_ptr) {
  ev_ptr->type = ev_type, ev_ptr->code = ev_code, ev_ptr->value = ev_value, ev_ptr->time.tv_sec = 0,
  ev_ptr->time.tv_usec = 0;
  // printf("%p\n", ev_ptr);
}

void tablet_input_event(int x, int y, int pressure, int btn) {
  struct input_event position_events[4];
  memset(&position_events, 0, sizeof(position_events));
  // struct input_event *position_events = (struct input_event *)malloc(sizeof(struct input_event) * 4);

  create_input(EV_KEY, BTN_TOOL_PEN, 1, &position_events[0]);
  create_input(EV_ABS, ABS_X, x + offset_x, &position_events[1]);
  create_input(EV_ABS, ABS_Y, y + offset_y, &position_events[2]);

  // printf("%p\n", &position_events[0]);
  // printf("%p\n", &position_events[1]);
  // printf("%p\n\n", &position_events[2]);

  // printf("p[1]:%d\n", position_events[1].value);
  // printf("%08b\n", btn & 0b00000111);

  switch (((btn & 0xff) & 0x07)) {
  case 0x01:
    click_value = 1;
    if (is_click == 0) {
      mbtn = BTN_LEFT;
      is_click = 1;
      position_events[3].type = EV_KEY;
      position_events[3].code = mbtn;
      position_events[3].value = click_value;
      position_events[3].time.tv_sec = 0;
      position_events[3].time.tv_usec = 0;
    }
    break;

  case 0x04:
    click_value = 1;
    if (is_click == 0) {
      mbtn = BTN_RIGHT;
      is_click = 1;
      position_events[3].type = EV_KEY;
      position_events[3].code = mbtn;
      position_events[3].value = click_value;
      position_events[3].time.tv_sec = 0;
      position_events[3].time.tv_usec = 0;
    }
    break;

  case 0x00:
    click_value = 0;
    if (is_click) {
      is_click = 0;
      position_events[3].type = EV_KEY;
      position_events[3].code = mbtn;
      position_events[3].value = click_value;
      position_events[3].time.tv_sec = 0;
      position_events[3].time.tv_usec = 0;
      mbtn = 0;
    }
  }

  int res_w = write(fd, position_events, sizeof(position_events));

  struct input_event sync_event;
  memset(&sync_event, 0, sizeof(sync_event));

  sync_event.type = EV_SYN;
  sync_event.value = 0;
  sync_event.code = SYN_REPORT;

  write(fd, &sync_event, sizeof(sync_event));
}

int area_boundary_clamp(double x, double y, double *px, double *py) {
  if (x > primary_width)
    *px = primary_width;

  if (x < 0)
    *px = 0;

  if (y < 0)
    *py = 0;

  if (y > primary_height)
    *py = primary_height;

  // if (x == 0 && y == 0) {
  //   return 0;
  // }

  if (*px == 0 && *py == 0) {
    return 0;
  }

  return 1;
}

void parse_tablet_buffer(struct tablet_config tablet) {
  int x = 0;
  int y = 0;
  double xS = 0;
  double yS = 0;
  int r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    r = read(fdn, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    if (buf[0] <= 0x10) {
      // print_hex_buffer(buf, r);
      x = (buf[tablet.xindex] & 0xff) | ((buf[tablet.xindex + 1] & 0xff) << 8);
      y = (buf[tablet.yindex] & 0xff) | ((buf[tablet.yindex + 1] & 0xff) << 8);

      xS = (x - tablet.left) * tablet.xscale;
      yS = (y - tablet.top) * tablet.yscale;

      // if ((buf[0] & 0xff) < 0x11) {
      if (area_boundary_clamp(xS, yS, &xS, &yS))
        tablet_input_event(xS, yS, 0, buf[1]);
      // }
    }
  }
}

void init_read(struct tablet_config tablet, struct display_config display_conf, const char *hidraw_path) {
  char tablet_path[32];
  strlcpy(tablet_path, hidraw_path, 32);

  offset_x = display_conf.offset_x;
  offset_y = display_conf.offset_y;
  primary_width = display_conf.primary_width;
  primary_height = display_conf.primary_height;

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

// int xpos_buffer[64];
// int ypos_buffer[64];
// usleep(10000);
// memset(&xpos_buffer, 0, sizeof(xpos_buffer));

// int area_boundary_clamp(uint *x, uint *y) {
// int area_boundary_clamp(double *x, double *y) {

void print_hex_buffer(uint8_t *buf, int len) {
  printf("\n");
  for (int i = 0; i < len; i++) {
    printf("%02hhx ", buf[i]);
    // printf("%d ", (buf[1] & (2 ^ i)) );
  }
}