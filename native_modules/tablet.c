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
#include <time.h>
#include <unistd.h>

int init_uinput(const char *name, int x_max, int y_max) {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    perror("uinput open err");
    exit(EXIT_FAILURE);
  }

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_POINTER);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE); // prevent left+right middle click
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_EVBIT, EV_ABS);

  struct uinput_setup utablet_setup;
  memset(&utablet_setup, 0, sizeof(utablet_setup));
  snprintf(utablet_setup.name, UINPUT_MAX_NAME_SIZE, "%s", name);

  utablet_setup.id.bustype = BUS_USB;
  utablet_setup.id.version = 0;
  utablet_setup.id.vendor = 0x0;
  utablet_setup.id.product = 0x0;
  ioctl(fd, UI_DEV_SETUP, &utablet_setup);

  struct uinput_abs_setup abs_xprops;
  memset(&abs_xprops, 0, sizeof(abs_xprops));

  abs_xprops.code = ABS_X;
  abs_xprops.absinfo.minimum = 0;
  abs_xprops.absinfo.maximum = x_max;
  ioctl(fd, UI_ABS_SETUP, &abs_xprops);

  struct uinput_abs_setup abs_yprops;
  memset(&abs_yprops, 0, sizeof(abs_yprops));

  abs_yprops.code = ABS_Y;
  abs_yprops.absinfo.minimum = 0;
  abs_yprops.absinfo.maximum = y_max;
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  ioctl(fd, UI_DEV_CREATE);
  printf("created uinput device:%s\n", name);

  return fd;
}

int is_click = 0;
int click_value = 0;
int mbtn = 0;
int btn_state = 0b00000000;
int last_btn_state = 0b11010000; // 0b00000000

#define PEN_BUTTON0 0b00000001
#define PEN_BUTTON1 0b00000010
#define PEN_BUTTON2 0b00000100
#define EVENT_SIZE 24 // sizeof(struct input_event)
// #define PEN_BUTTON2 0b00100000 // 0x10 // buf[13]

size_t create_input(uint16_t ev_type, uint16_t ev_code, int ev_value, struct input_event *ev_ptr) {
  ev_ptr->type = ev_type, ev_ptr->code = ev_code, ev_ptr->value = ev_value, ev_ptr->time.tv_sec = 0, ev_ptr->time.tv_usec = 0;
  return EVENT_SIZE;
}

void tabletbtn_input_event(int tablet_fd, int x, int y, int btn) {
  size_t num_bytes = 0; // count bytes to write
  struct input_event position_events[5];
  memset(&position_events, 0, sizeof(position_events));

  struct input_event sync_event;
  memset(&sync_event, 0, sizeof(sync_event));

  sync_event.type = EV_SYN;
  sync_event.value = 0;
  sync_event.code = SYN_REPORT;

  num_bytes += create_input(EV_KEY, BTN_TOOL_PEN, 1, &position_events[0]);
  num_bytes += create_input(EV_ABS, ABS_X, x, &position_events[1]);
  num_bytes += create_input(EV_ABS, ABS_Y, y, &position_events[2]);

  if (btn != last_btn_state) {
    if (((btn ^ last_btn_state) & PEN_BUTTON0)) {
      num_bytes += create_input(EV_KEY, BTN_LEFT, btn & PEN_BUTTON0, &position_events[num_bytes / EVENT_SIZE]);
    }

    if (((btn ^ last_btn_state) & PEN_BUTTON1)) {
      num_bytes += create_input(EV_KEY, BTN_RIGHT, (btn & PEN_BUTTON1) >> 1, &position_events[num_bytes / EVENT_SIZE]);
    }

    if (((btn ^ last_btn_state) & PEN_BUTTON2)) {
      num_bytes += create_input(EV_KEY, BTN_RIGHT, (btn & PEN_BUTTON2) >> 2, &position_events[num_bytes / EVENT_SIZE]);
    }

    DEBUG_PRINT("btn_state changed:%08b, changed buttons:%08b\n", btn, btn ^ last_btn_state);
    DEBUG_PRINT("btn_state: %08b\n", ((btn ^ last_btn_state) & 0b00000111));
  }

#ifndef DEBUG_EVENTS
  write(tablet_fd, position_events, num_bytes);
  write(tablet_fd, &sync_event, sizeof(sync_event));
#else
  ssize_t res_w = write(tablet_fd, position_events, num_bytes);

  if (res_w < 0)
    perror("\nwrite error");

  ssize_t b = write(tablet_fd, &sync_event, sizeof(sync_event));
  printf("x:%d, y:%d, nbytes:%zd syncwrite:%zd fd:%d\n", x, y, res_w, b, tablet_fd);
#endif
  last_btn_state = btn;
}

int area_boundary_clamp(unsigned int max_width, unsigned int max_height, double *x, double *y) {
  if (*x > max_width)
    *x = max_width;

  if (*x < 0)
    *x = 0;

  if (*y < 0)
    *y = 0;

  if (*y > max_height)
    *y = max_height;

  if (*x == 0 && *y == 0)
    return 0;

  return 1;
}

void parse_tablet_buffer(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;

  ssize_t r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    if (buf[0] <= 0x10) {
      x = (buf[tablet.xindex]) | ((buf[tablet.xindex + 1]) << 8);
      y = (buf[tablet.yindex]) | ((buf[tablet.yindex + 1]) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled))
        tabletbtn_input_event(tablet_fd, (int)x_scaled + display.offset_x, (int)y_scaled + display.offset_y, buf[tablet.bindex]);
    }
  }
}

#define REPORT_RATE 133

void parse_tablet_buffer_interpolated(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;
  int x_prev = -1;
  int y_prev = -1;

  double double_report_delay = ((1000.0 / REPORT_RATE) / 2.0) * 1000000;
  struct timespec ts = {.tv_sec = 0, .tv_nsec = (long)double_report_delay}; // 3759398

  ssize_t r;
  int active = 1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    if (buf[0] <= 0x10) {
      x = (buf[tablet.xindex]) | ((buf[tablet.xindex + 1]) << 8);
      y = (buf[tablet.yindex]) | ((buf[tablet.yindex + 1]) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      if (x_prev != -1) {
        double x2 = (int)(((x_scaled + x_prev)) / 2);
        double y2 = (int)((y_scaled + y_prev) / 2);

        if (area_boundary_clamp(display.primary_width, display.primary_height, &x2, &y2))
          tabletbtn_input_event(tablet_fd, (int)x2 + display.offset_x, (int)y2 + display.offset_y, buf[tablet.bindex]);

        nanosleep(&ts, NULL);

        if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled))
          tabletbtn_input_event(tablet_fd, (int)x_scaled + display.offset_x, (int)y_scaled + display.offset_y, buf[tablet.bindex]);

      } else {
        if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled))
          tabletbtn_input_event(tablet_fd, (int)x_scaled + display.offset_x, (int)y_scaled + display.offset_y, buf[tablet.bindex]);
      }

      x_prev = (int)x_scaled;
      y_prev = (int)y_scaled;
    } else {
      x_prev = -1;
      y_prev = -1;
    }
  }
}

void parse_tablet_buffer_avg(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;
  ssize_t r;
  int active = 1;

  int xpos_buffer[] = {-1, -1, -1, -1};
  int ypos_buffer[] = {-1, -1, -1, -1};
  int xprevious = -1;
  int yprevious = -1;

  uint8_t buf[64];
  memset(buf, 0x0, sizeof(buf));

  while (active) {
    r = read(buffer_fd, buf, 16);

    if (r < 0) {
      perror("\nread err");
      exit(EXIT_FAILURE);
    }

    if (buf[0] <= 0x10) {
      // print_hex_buffer(buf, r);
      x = (buf[tablet.xindex] & 0xff) | ((buf[tablet.xindex + 1] & 0xff) << 8);
      y = (buf[tablet.yindex] & 0xff) | ((buf[tablet.yindex + 1] & 0xff) << 8);

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      // if ((buf[0] & 0xff) < 0x11) {
      if (area_boundary_clamp(display.primary_width, display.primary_height, &x_scaled, &y_scaled)) {
        x = (int)x_scaled + display.offset_x;
        y = (int)y_scaled + display.offset_y;
        int xdiff = abs(xprevious - x);
        int ydiff = abs(yprevious - y);
        // printf("diff: %d\n", xdiff);

        if (xdiff <= 3)
          x = xprevious;
        else if (xdiff > 3)
          x = ((xprevious + x) / 2);

        if (ydiff <= 3)
          y = yprevious;
        else if (ydiff > 3)
          y = ((yprevious + y) / 2);
        // check -1 first?

        tabletbtn_input_event(tablet_fd, x, y, buf[1]);
        xprevious = x;
        yprevious = y;
      }
    }
  }
}

int init_read_buffer(const char *hidraw_path) {
  char tabletbuf_path[32];
  strlcpy(tabletbuf_path, hidraw_path, 32);

  int fdn = open(tabletbuf_path, O_RDONLY | O_SYNC);
  // fdn = open(tablet_path, O_RDONLY | O_NONBLOCK);

  if (fdn < 0) {
    printf("Unable to open device with path %s:", tabletbuf_path);
    perror("\nread err");
    exit(EXIT_FAILURE);
  }

  printf("reading reports from: %s\n", tabletbuf_path);

  return fdn;
}

void init_tablet(const char *name, const char *hidraw_path, struct tablet_config tablet, struct display_config display) {
  int tablet_input_fd = init_uinput(name, display.total_width, display.total_height);
  int buffer_fd = init_read_buffer(hidraw_path);

  parse_tablet_buffer(buffer_fd, tablet_input_fd, tablet, display);
  // parse_tablet_buffer_interpolated(buffer_fd, tablet_input_fd, tablet, display);
}

void tablet_input_event(int tablet_fd, int x, int y, int btn) {
  struct input_event position_events[4];
  memset(&position_events, 0, sizeof(position_events));

  create_input(EV_KEY, BTN_TOOL_PEN, 1, &position_events[0]);
  create_input(EV_ABS, ABS_X, x, &position_events[1]);
  create_input(EV_ABS, ABS_Y, y, &position_events[2]);

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

  write(tablet_fd, position_events, sizeof(position_events));

  struct input_event sync_event;
  memset(&sync_event, 0, sizeof(sync_event));

  sync_event.type = EV_SYN;
  sync_event.value = 0;
  sync_event.code = SYN_REPORT;

  write(tablet_fd, &sync_event, sizeof(sync_event));
}

void print_hex_buffer(uint8_t *buf, int len) {
  printf("\n");
  for (int i = 0; i < len; i++) {
    printf("%02hhx ", buf[i]);
  }
}

int init_uinput_tablet(const char *name, int x_max, int y_max) {
  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  if (fd < 0) {
    printf("error opening uinput\n");
    perror("uinput open err");
    exit(EXIT_FAILURE);
  }

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_POINTER);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

  ioctl(fd, UI_SET_EVBIT, EV_ABS);
  ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
  ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS);
  ioctl(fd, UI_SET_KEYBIT, BTN_STYLUS2);

  // ioctl(fd, UI_SET_ABSBIT, ABS_X);
  // ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  struct uinput_setup utablet_setup;
  memset(&utablet_setup, 0, sizeof(utablet_setup));
  snprintf(utablet_setup.name, UINPUT_MAX_NAME_SIZE, "%s", name);

  utablet_setup.id.bustype = BUS_USB;
  utablet_setup.id.version = 0;
  utablet_setup.id.vendor = 0x0;
  utablet_setup.id.product = 0x0;
  ioctl(fd, UI_DEV_SETUP, &utablet_setup);

  struct uinput_abs_setup abs_xprops;
  memset(&abs_xprops, 0, sizeof(abs_xprops));

  abs_xprops.code = ABS_X;
  abs_xprops.absinfo.minimum = 0;
  abs_xprops.absinfo.maximum = x_max;
  abs_xprops.absinfo.resolution = 100;
  ioctl(fd, UI_ABS_SETUP, &abs_xprops);

  struct uinput_abs_setup abs_yprops;
  memset(&abs_yprops, 0, sizeof(abs_yprops));

  abs_yprops.code = ABS_Y;
  abs_yprops.absinfo.minimum = 0;
  abs_yprops.absinfo.maximum = y_max;
  abs_yprops.absinfo.resolution = 100;
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  ioctl(fd, UI_DEV_CREATE);
  printf("created uinput tablet device:%s\n", name);

  return fd;
}
