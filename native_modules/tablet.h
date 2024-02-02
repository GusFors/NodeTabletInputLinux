#ifndef TABLET_H
#define TABLET_H
#include <string>
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

void init_uinput(std::string name, int32_t xMax, int32_t yMax);
void tablet_input_event(int32_t x, int32_t y, int32_t pressure, int32_t btn);
void parse_tablet_buffer(tablet_config tablet);
void init_read(tablet_config tablet, display_config display_conf, const char *hidraw_path);

#ifdef __cplusplus
}
#endif

#endif