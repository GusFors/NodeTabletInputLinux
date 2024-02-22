#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../display.h"
#include "../tablet.h"

struct tablet_config get_tablet_config(const char *tablet_name) {
  struct tablet_config tablet_conf;
  // write values directly here for now
  tablet_conf.left = 3600;
  tablet_conf.top = 1406;
  tablet_conf.xindex = 2;
  tablet_conf.yindex = 4;
  tablet_conf.xscale = 0.32;
  tablet_conf.yscale = 0.32;

  return tablet_conf;
}

struct display_config get_display_config() {
  struct display_config display_conf;
  display_conf.offset_x = 2560;
  display_conf.offset_y = 0;
  display_conf.primary_height = 1440;
  display_conf.primary_width = 2560;
  display_conf.total_height = 1440;
  display_conf.total_width = 7680;

  return display_conf;
}