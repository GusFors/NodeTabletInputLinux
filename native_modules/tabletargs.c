#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "display.h"
#include "tablet.h"

int main(int argc, char *argv[]) {
  const char *hidraw_path = argv[1];
  const char *device_name = argv[2];
  printf("hidraw_path: %s\n", hidraw_path);

  int left = strtol(argv[3], NULL, 0);
  int top = strtol(argv[4], NULL, 0);
  int right = strtol(argv[5], NULL, 0);
  int bottom = strtol(argv[6], NULL, 0);

  printf("left:%d\n", left);

  // double xscale = strtod(argv[5], NULL);
  // double yscale = strtod(argv[6], NULL);

  int xbuf_index = strtol(argv[7], NULL, 0);
  int ybuf_index = strtol(argv[8], NULL, 0);

  printf("xposition:%d\n", xbuf_index);
  printf("yposition:%d\n\n", ybuf_index);

  struct display_config display_conf;
  get_full_display_config(&display_conf);

  double xscale = (double)display_conf.primary_width / (right - left);
  double yscale = (double)display_conf.primary_height / (bottom - top);

  printf("xscale:%f\n\n", xscale);

  struct tablet_config tablet;
  tablet.left = left;
  tablet.top = top;
  tablet.right = right;
  tablet.bottom = bottom;
  tablet.xscale = xscale;
  tablet.yscale = yscale;
  tablet.xindex = xbuf_index;
  tablet.yindex = ybuf_index;

  init_uinput(device_name, display_conf.total_width, display_conf.total_height);
  init_read(tablet, display_conf, hidraw_path);

  return 0;
}