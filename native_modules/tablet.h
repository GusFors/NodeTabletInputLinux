#ifndef TABLET_H
#define TABLET_H
#include <linux/input.h>
#include <stdint.h>
#include "display.h"

enum parser_type {
  STANDARD_PARSER = 0,
  DOUBLE_REPORT_PARSER = 1,
  AVERAGE_REPORT_PARSER = 2,
  AVERAGE_BUFFERED_REPORT_PARSER = 3,
};

struct tablet_config {
  int left;
  int right;
  int top;
  int bottom;
  uint8_t xindex;
  uint8_t yindex;
  uint8_t bindex;
  uint8_t parser;
  double xscale;
  double yscale;
};

#ifndef DEBUG_LOGGING
#define DEBUG_PRINT(...) while (0)
#else
#define DEBUG_PRINT(...)                                                                                                                             \
  do {                                                                                                                                               \
    printf(__VA_ARGS__);                                                                                                                             \
  } while (0)
#endif

#ifndef DEBUG_RAWREPORT
#define DEBUG_REPORT(...) while (0)
#else
#define DEBUG_REPORT(...)                                                                                                                            \
  do {                                                                                                                                               \
    print_hex_buffer(__VA_ARGS__);                                                                                                                   \
  } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void init_tablet(const char *name, const char *hidraw_path, struct tablet_config tablet, struct display_config display);
int init_uinput(const char *name, int x_max, int y_max);
void tabletbtn_input_event(int tablet_fd, int x, int y, int btn);
void tablet_input_event(int tablet_fd, int x, int y, int btn);
void parse_tablet_buffer(int buffer_fd, int tablet_fd, struct tablet_config tablet, struct display_config display_conf);
int init_read_buffer(const char *hidraw_path);
void print_hex_buffer(uint8_t *buf, unsigned int len);
int area_boundary_clamp(unsigned int max_width, unsigned int max_height, double *x, double *y);
size_t create_input(uint16_t ev_type, uint16_t ev_code, int ev_value, struct input_event *ev_ptr);

#ifdef __cplusplus
}
#endif

#endif
