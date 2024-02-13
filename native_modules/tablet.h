#ifndef TABLET_H
#define TABLET_H
#include <linux/input.h>
#include <stdint.h>
#include "display.h"

struct tablet_config {
  int left;
  int top;
  int xindex;
  int yindex;
  double xscale;
  double yscale;
  // char *hidraw_path;
};

#ifdef __cplusplus
extern "C" {
#endif

void init_uinput(const char *name, int x_max, int y_max);
void tablet_input_event(int x, int y, int pressure, int btn);
void parse_tablet_buffer(struct tablet_config tablet);
void init_read(struct tablet_config tablet, struct display_config display_conf, const char *hidraw_path);
void print_hex_buffer(uint8_t *buf, int len);
int area_boundary_clamp(double x, double y, double *px, double *py);
int create_input(int ev_type, int ev_code, int ev_value, struct input_event *ev_ptr);

#ifdef __cplusplus
}
#endif

#endif