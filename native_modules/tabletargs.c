#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"
#include "tablet.h"
#include "./conf/device_detector.h"
#include "./conf/config_handler.h"

int main(int argc, char *argv[]) {
  struct device_info tabletdev = detect_tablet();
  printf("path: %s\n", tabletdev.hidraw_path);
  struct tablet_config tablet = get_tablet_config(tabletdev.vendor, tabletdev.product);

  const char *hidraw_path = argv[1];
  const char *device_name = argv[2];
  printf("hidraw_path: %s\n", hidraw_path);

  int left = strtol(argv[3], NULL, 0);
  int top = strtol(argv[4], NULL, 0);
  int right = strtol(argv[5], NULL, 0);
  int bottom = strtol(argv[6], NULL, 0);

  // double xscale = strtod(argv[5], NULL);
  // double yscale = strtod(argv[6], NULL);

  int xbuf_index = strtol(argv[7], NULL, 0);
  int ybuf_index = strtol(argv[8], NULL, 0);

  struct display_config display_conf;
  get_full_display_config(&display_conf);

  double xscale = (double)display_conf.primary_width / (right - left);
  double yscale = (double)display_conf.primary_height / (bottom - top);

  // struct tablet_config tablet;
  // tablet.left = left;
  // tablet.top = top;
  // tablet.right = right;
  // tablet.bottom = bottom;
  // tablet.xscale = xscale;
  // tablet.yscale = yscale;
  // tablet.xindex = xbuf_index;
  // tablet.yindex = ybuf_index;

  printf("left:%d\n", tablet.left);
  printf("right:%d\n", tablet.right);
  printf("top:%d\n", tablet.top);
  printf("bottom:%d\n", tablet.bottom);
  printf("xposition:%d\n", tablet.xindex);
  printf("yposition:%d\n", tablet.yindex);
  printf("xscale:%f\n\n", tablet.xscale);

  init_tablet(device_name, hidraw_path, tablet, display_conf);
  // init_uinput(device_name, display_conf.total_width, display_conf.total_height);
  // init_read(tablet, display_conf, hidraw_path);

  return 0;
}