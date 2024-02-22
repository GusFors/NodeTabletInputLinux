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
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE); // prevent left+right middle click
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_EVBIT, EV_ABS);

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
  // abs_xprops.absinfo = (input_absinfo){.value = 0, .minimum = 0, .maximum = x_max, .fuzz = 0, .flat = 0, .resolution
  // = 0};
  ioctl(fd, UI_ABS_SETUP, &abs_xprops);

  struct uinput_abs_setup abs_yprops;
  memset(&abs_yprops, 0, sizeof(abs_yprops));

  abs_yprops.code = ABS_Y;
  abs_yprops.absinfo.minimum = 0;
  abs_yprops.absinfo.maximum = y_max;
  ioctl(fd, UI_ABS_SETUP, &abs_yprops);

  // write(fd, &utablet_setup, sizeof(utablet_setup));
  ioctl(fd, UI_DEV_CREATE);
  printf("created uinput device:%s\n", name);
}

int is_click = 0;
int click_value = 0;
int mbtn = 0;
int offset_x;
int offset_y;
int primary_height;
int primary_width;
int btn_state = 0b00000000;
int last_btn_state = 0b11010000;

#define PEN_BUTTON0 0b00000001
#define PEN_BUTTON1 0b00000010
#define PEN_BUTTON2 0b00000100
#define EVENT_SIZE 24 // sizeof(struct input_event)

int create_input(int ev_type, int ev_code, int ev_value, struct input_event *ev_ptr) {
  ev_ptr->type = ev_type, ev_ptr->code = ev_code, ev_ptr->value = ev_value, ev_ptr->time.tv_sec = 0,
  ev_ptr->time.tv_usec = 0;
  return EVENT_SIZE;
}

void tabletbtn_input_event(int x, int y, int pressure, int btn) {
  int num_bytes = 0; // count bytes to write
  struct input_event position_events[5];
  memset(&position_events, 0, sizeof(position_events));

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
    // printf("btn_state changed:%08b, changed buttons:%08b\n", btn, btn ^ last_btn_state);
    // printf("btn_state: %08b\n", ((btn ^ last_btn_state) & 0b00000111));
  }

  int res_w = write(fd, position_events, num_bytes);
  last_btn_state = btn;

  struct input_event sync_event;
  memset(&sync_event, 0, sizeof(sync_event));

  sync_event.type = EV_SYN;
  sync_event.value = 0;
  sync_event.code = SYN_REPORT;

  int b = write(fd, &sync_event, sizeof(sync_event));
  // printf("x:%d, y:%d, nbytes:%d syncwrite:%d fd:%d\n", x, y, res_w, b, fd);
}

void tablet_input_event(int x, int y, int pressure, int btn) {
  struct input_event position_events[4];
  memset(&position_events, 0, sizeof(position_events));
  // struct input_event *position_events = (struct input_event *)malloc(sizeof(struct input_event) * 4);

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

  if (*px == 0 && *py == 0) {
    return 0;
  }

  return 1;
}

void parse_tablet_buffer(struct tablet_config tablet, struct display_config display) {
  int x = 0;
  int y = 0;
  double x_scaled = 0;
  double y_scaled = 0;
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

      x_scaled = (x - tablet.left) * tablet.xscale;
      y_scaled = (y - tablet.top) * tablet.yscale;

      // if ((buf[0] & 0xff) < 0x11) {
      if (area_boundary_clamp(x_scaled, y_scaled, &x_scaled, &y_scaled))
        tabletbtn_input_event(x_scaled + display.offset_x, y_scaled + display.offset_y, 0, buf[1]);
    }
  }
}

void init_read(struct tablet_config tablet, struct display_config display_conf, const char *hidraw_path) {
  char tablet_path[32];
  strlcpy(tablet_path, hidraw_path, 32);

  primary_width = display_conf.primary_width;
  primary_height = display_conf.primary_height;

  fdn = open(tablet_path, O_RDONLY | O_SYNC);
  // fdn = open(tablet_path, O_RDONLY | O_NONBLOCK);

  if (fdn < 0) {
    printf("Unable to open device with path %s:", tablet_path);
    perror("\nread err");
    exit(EXIT_FAILURE);
  }

  printf("reading reports from: %s\n", tablet_path);

  parse_tablet_buffer(tablet, display_conf);
}

// int xpos_buffer[64];
// int ypos_buffer[64];
// usleep(10000);
// memset(&xpos_buffer, 0, sizeof(xpos_buffer));

// int area_boundary_clamp(uint *x, uint *y) {
// int area_boundary_clamp(double *x, double *y) {

// switch (((btn ^ last_btn_state) & 0b00000111)) {
//   case 0b00000001:
//     btn_state = btn_state | (btn & PEN_BUTTON);
//     num_bytes += create_input(EV_KEY, BTN_LEFT, btn & PEN_BUTTON, &position_events[num_bytes / EVENT_SIZE]);
//   // fall-through
//   case 0b00000100:
//       btn_state = btn_state | (btn & PEN_BUTTON2);
//       num_bytes += create_input(EV_KEY, BTN_RIGHT, btn & PEN_BUTTON2, &position_events[num_bytes / EVENT_SIZE]);
// }

// if ((!(btn_state & PEN_BUTTON2) && (btn & PEN_BUTTON2))) {
//   btn_state = btn_state | (btn & PEN_BUTTON2);
//   num_bytes += create_input(EV_KEY, BTN_RIGHT, btn & PEN_BUTTON2, &position_events[num_bytes / EVENT_SIZE]);
// } else if ((btn_state & PEN_BUTTON2) && !(btn & PEN_BUTTON2)) {
//   btn_state = btn_state & ~PEN_BUTTON2;
//   num_bytes += create_input(EV_KEY, BTN_RIGHT, btn & PEN_BUTTON2, &position_events[num_bytes / EVENT_SIZE]);
// }

// printf("pen:%d btn1:%d btn2:%d num bytes:%d, res w:%d\n", btn & 0b00000001, btn & 0b00000010, (btn >> 2) & 1,
//        num_bytes, res_w);

// if ((!(btn_state & 0b00000100) && (btn & 0b00000100))) {
//   // btn_state = btn_state | btn;
//   btn_state = btn_state | (btn & 0b00000100);
// } else if ((btn_state & 0b00000100) && !(btn & 0b00000100)) {
//   // btn_state = btn_state & ~btn;
//   btn_state = btn_state & ~0b00000100;
// }

void print_hex_buffer(uint8_t *buf, int len) {
  printf("\n");
  for (int i = 0; i < len; i++) {
    printf("%02hhx ", buf[i]);
  }
}