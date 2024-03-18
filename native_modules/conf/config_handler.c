#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../display.h"
#include "../tablet.h"

struct tablet_config get_tablet_config(int vendor, int product) {
  FILE *conf_file;
  conf_file = fopen("./conf/tablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(1);
  }

  char line[128] = "";
  // int vendor = 1386;
  // int product = 782;
  struct tablet_config tablet_conf;
  int vend_match = 0;
  int prod_match = 0;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, "=");

    if (strvalue != NULL) {
      int value = strtol(strvalue, NULL, 0);

      if (strcmp("vendor", key) == 0 && value == vendor)
        vend_match = 1;

      if (strcmp("product", key) == 0 && value == product)
        prod_match = 1;

      printf("%s=%d\n", key, value);
    }
  }
  printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);

  fclose(conf_file);

  // write values directly here for now
  tablet_conf.left = 3600;
  tablet_conf.right = 11600;
  tablet_conf.top = 1406;
  tablet_conf.bottom = 5906;
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
