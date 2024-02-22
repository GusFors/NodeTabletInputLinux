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

  // Display *display = XOpenDisplay(NULL);
  // XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  // XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  // int xoffset = get_primary_monitor_xoffset(display, mon_res, mon_info);
  // int yoffset = get_primary_monitor_yoffset(display, mon_res, mon_info);
  // int primary_width = get_primary_monitor_width(display, mon_res, mon_info);
  // int primary_height = get_primary_monitor_height(display, mon_res, mon_info);

  // int x_display_maxval = get_displays_total_width(display);
  // int y_display_maxval = get_displays_total_height(display);

  // free_xresources(mon_res, mon_info);
  // int v = close_display(display);

  // printf("x_display_maxval:%d\n", x_display_maxval);
  // printf("y_display_maxval:%d\n", y_display_maxval);

  int left = strtol(argv[3], NULL, 0);
  int top = strtol(argv[4], NULL, 0);

  printf("left:%d\n", left);

  double xscale = strtod(argv[5], NULL);
  double yscale = strtod(argv[6], NULL);

  printf("xscale:%f\n\n", xscale);

  // printf("xWidth:%d\n", primary_width);
  // printf("xoffset:%d\n", xoffset);

  int xbuf_index = strtol(argv[7], NULL, 0);
  int ybuf_index = strtol(argv[8], NULL, 0);

  printf("xposition:%d\n", xbuf_index);
  printf("yposition:%d\n\n", ybuf_index);

  struct tablet_config tablet;
  tablet.left = left;
  tablet.top = top;
  tablet.xscale = xscale;
  tablet.yscale = yscale;
  tablet.xindex = xbuf_index;
  tablet.yindex = ybuf_index;

  // struct display_config display_conf = get_display_config();
  struct display_config display_conf;
  get_full_display_config(&display_conf);

  init_uinput(device_name, display_conf.total_width, display_conf.total_height);
  init_read(tablet, display_conf, hidraw_path);

  // struct display_config display_conf;
  // display_conf.offset_x = xoffset;
  // display_conf.offset_y = yoffset;
  // display_conf.primary_height = primary_height;
  // display_conf.primary_width = primary_width;

  // init_uinput(device_name, x_display_maxval, y_display_maxval);
  // init_read(tablet, display_conf, hidraw_path);

  return 0;
}