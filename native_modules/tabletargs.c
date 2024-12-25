#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "display.h"
#include "tablet.h"
#include "./conf/device_detector.h"
#include "./conf/config_handler.h"

int main(int argc, char *argv[]) {
  bool verbose = false;

  for (int i = 1; i < argc; i++) {
    printf("arg[%d]: %s\n", i, argv[i]);

    if (strncmp(argv[i], "-v", sizeof("-v")) == 0)
      verbose = 1;
  }

  char matched_name[64];
  struct device_info tabletdev = detect_tablet(verbose);
  struct tablet_config tablet = get_tablet_mmconfig(tabletdev.vendor, tabletdev.product, matched_name);
  printf("path: %s\n", tabletdev.hidraw_path);

  struct display_config display_conf;
  get_full_display_config(&display_conf);

  tablet.xscale = (double)display_conf.primary_width / (tablet.right - tablet.left);
  tablet.yscale = (double)display_conf.primary_height / (tablet.bottom - tablet.top);

  printf("left:%d\n", tablet.left);
  printf("right:%d\n", tablet.right);
  printf("top:%d\n", tablet.top);
  printf("bottom:%d\n", tablet.bottom);
  printf("xposition:%d\n", tablet.xindex);
  printf("yposition:%d\n", tablet.yindex);
  printf("bposition:%d\n", tablet.bindex);
  printf("xscale:%f\n\n", tablet.xscale);

  init_tablet(matched_name, tabletdev.hidraw_path, tablet, display_conf);

  return 0;
}
