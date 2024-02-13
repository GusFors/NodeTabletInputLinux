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

  int xOffset = get_primary_monitor_xoffset();
  int yOffset = get_primary_monitor_yoffset();
  int xPrimaryWidth = get_primary_monitor_width();
  int yPrimaryHeight = get_primary_monitor_height();

  int x_display_maxval = get_displays_total_width();
  int y_display_maxval = get_displays_total_height();
  int v = close_display();

  printf("x_display_maxval:%d\n", x_display_maxval);
  printf("y_display_maxval:%d\n", y_display_maxval);

  int left = strtol(argv[3], NULL, 0);
  int top = strtol(argv[4], NULL, 0);

  printf("left:%d\n", left);

  double xScale = strtod(argv[5], NULL);
  double yScale = strtod(argv[6], NULL);

  printf("xScale:%f\n\n", xScale);

  printf("xWidth:%d\n", xPrimaryWidth);
  printf("xOffset:%d\n", xOffset);

  int xBufferPos = strtol(argv[7], NULL, 0);
  int yBufferPos = strtol(argv[8], NULL, 0);

  printf("xPosition:%d\n", xBufferPos);
  printf("yPosition:%d\n\n", yBufferPos);

  struct tablet_config tablet;
  tablet.left = left;
  tablet.top = top;
  tablet.xscale = xScale;
  tablet.yscale = yScale;
  tablet.xindex = xBufferPos;
  tablet.yindex = yBufferPos;

  struct display_config display_conf;
  display_conf.offset_x = xOffset;
  display_conf.offset_y = yOffset;
  display_conf.primary_height = yPrimaryHeight;
  display_conf.primary_width = xPrimaryWidth;

  init_uinput(device_name, x_display_maxval, y_display_maxval);
  init_read(tablet, display_conf, hidraw_path);

  return 0;
}