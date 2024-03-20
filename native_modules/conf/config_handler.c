#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../display.h"
#include "../tablet.h"
#include "config_handler.h"

struct tablet_config get_tablet_config(int vendor, int product) {
  FILE *conf_file;
  conf_file = fopen("./conf/tablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(1);
  }

  struct tablet_config tablet_conf;
  char line[128] = "";
  int vend_match = 0;
  int prod_match = 0;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, "=");

    if (vend_match == 1 && prod_match == 1 && strvalue == NULL) {
      printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);
      break;
    }

    if (strvalue != NULL) {
      int value = strtol(strvalue, NULL, 0);

      if (strcmp("left", key) == 0) {
        tablet_conf.left = value;
      } else if (strcmp("right", key) == 0) {
        tablet_conf.right = value;
      } else if (strcmp("top", key) == 0) {
        tablet_conf.top = value;
      } else if (strcmp("bottom", key) == 0) {
        tablet_conf.bottom = value;
      } else if (strcmp("xindex", key) == 0) {
        tablet_conf.xindex = value;
      } else if (strcmp("yindex", key) == 0) {
        tablet_conf.yindex = value;
      } else if (strcmp("vendor", key) == 0 && value == vendor) {
        vend_match = 1;
      } else if (strcmp("product", key) == 0 && value == product) {
        prod_match = 1;
      }

      printf("%s=%d\n", key, value);
    }
  }

  fclose(conf_file);

  if (vend_match == 0 || prod_match == 0) {
    printf("\nfailed to find a matching tablet config\n");
    exit(0);
  }

  // write values directly here for now
  tablet_conf.xscale = 0.32;
  tablet_conf.yscale = 0.32;
  // tablet_conf.left = 3600;
  // tablet_conf.right = 11600;
  // tablet_conf.top = 1406;
  // tablet_conf.bottom = 5906;
  // tablet_conf.xindex = 2;
  // tablet_conf.yindex = 4;

  return tablet_conf;
}

struct tablet_config get_tablet_mmconfig(int vendor, int product) {
  FILE *conf_file;
  conf_file = fopen("./conf/mmtablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(1);
  }

  struct tablet_config tablet_conf;
  char line[128] = "";
  int vend_match = 0;
  int prod_match = 0;
  double w = 0;
  double h = 0;
  double x = 0;
  double y = 0;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, "=");

    if (vend_match == 1 && prod_match == 1 && strvalue == NULL) {
      printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);
      break;
    }

    if (strvalue != NULL) {
      double value = strtod(strvalue, NULL);

      // check trunc
      if (strcmp("width", key) == 0) {
        w = value;
      } else if (strcmp("height", key) == 0) {
        h = value;
      } else if (strcmp("xoffset", key) == 0) {
        x = value;
      } else if (strcmp("yoffset", key) == 0) {
        y = value;
      } else if (strcmp("xindex", key) == 0) {
        tablet_conf.xindex = value;
      } else if (strcmp("yindex", key) == 0) {
        tablet_conf.yindex = value;
      } else if (strcmp("vendor", key) == 0 && value == vendor) {
        vend_match = 1;
      } else if (strcmp("product", key) == 0 && value == product) {
        prod_match = 1;
      }

      printf("%s=%f\n", key, value);
    }
  }

  fclose(conf_file);

  if (vend_match == 0 || prod_match == 0) {
    printf("\nfailed to find a matching tablet config\n");
    exit(0);
  }

  int left = (x - w / 2) * 100;
  int right = (x + w / 2) * 100;
  int top = (y - h / 2) * 100;
  int bottom = (y + h / 2) * 100;

  tablet_conf.left = left;
  tablet_conf.right = right;
  tablet_conf.top = top;
  tablet_conf.bottom = bottom;
  tablet_conf.xscale = 0.32;
  tablet_conf.yscale = 0.32;

  printf("\nconverted config\n");
  print_tablet_config(tablet_conf);

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

void print_tablet_config(struct tablet_config cfg) {
  printf("left: %d\nright: %d\n", cfg.left, cfg.right);
  printf("top: %d\nbottom: %d\n", cfg.top, cfg.bottom);
}