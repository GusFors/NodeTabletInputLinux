#ifndef TABLET_H
#define TABLET_H
#include <linux/input.h>
#include <stdint.h>
#include "display.h"

struct tablet_config {
  int left;
  int right;
  int top;
  int bottom;
  int xindex;
  int yindex;
  int bindex;
  double xscale;
  double yscale;
};

#ifdef __cplusplus
extern "C" {
#endif

void init_tablet(const char *name, const char *hidraw_path, struct tablet_config tablet, struct display_config display);
int init_uinput(const char *name, int x_max, int y_max);
void tabletbtn_input_event(int tablet_fd, int x, int y, int pressure, int btn);
void tablet_input_event(int tablet_fd, int x, int y, int pressure, int btn);
void parse_tablet_buffer(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display_conf);
int init_read_buffer(const char *hidraw_path);
void print_hex_buffer(uint8_t *buf, int len);
int area_boundary_clamp(int max_width, int max_height, double x, double y, double *px, double *py);
int create_input(int ev_type, int ev_code, int ev_value, struct input_event *ev_ptr);

#ifdef __cplusplus
}
#endif

#endif