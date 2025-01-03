#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "../display.h"
#include "../tablet.h"
#include "config_handler.h"

struct tablet_config get_tablet_mmconfig(int vendor, int product, char *matched_name) {
  FILE *conf_file;
  conf_file = fopen("./conf/mmtablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(1);
  }

  struct tablet_config tablet_conf;
  char line[128] = "";
  bool vend_match = false;
  bool prod_match = false;
  double w = 0;
  double h = 0;
  double x = 0;
  double y = 0;
  uint16_t res = 100;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, "=");

    if (vend_match == 1 && prod_match == 1 && strvalue == NULL)
      break;

    if (line[0] == '[') {
      printf("\nNew conf entry: %s", key);
      strlcpy(matched_name, "Absolute uinput Tablet ", 64);

      size_t len = strlen(key);
      if (key[len - 1] == '\n')
        key[len - 1] = '\0';

      strlcat(matched_name, key, 64);
      vend_match = false;
      prod_match = false;
      memset(&tablet_conf, 0, sizeof(tablet_conf));
    }

    if (strvalue != NULL) {
      double value = strtod(strvalue, NULL);

      if (strcmp("width", key) == 0) {
        w = value;
      } else if (strcmp("height", key) == 0) {
        h = value;
      } else if (strcmp("xoffset", key) == 0) {
        x = value;
      } else if (strcmp("yoffset", key) == 0) {
        y = value;
      } else if (strcmp("xindex", key) == 0) {
        tablet_conf.xindex = (uint8_t)value;
      } else if (strcmp("yindex", key) == 0) {
        tablet_conf.yindex = (uint8_t)value;
      } else if (strcmp("bindex", key) == 0) {
        tablet_conf.bindex = (uint8_t)value;
      } else if (strcmp("vendor", key) == 0 && value == vendor) {
        vend_match = true;
      } else if (strcmp("product", key) == 0 && value == product) {
        prod_match = true;
      } else if (strcmp("res", key) == 0) {
        res = (uint16_t)value;
      }

      printf("%s=%f\n", key, value);
    }
  }

  fclose(conf_file);

  if (vend_match == 1 && prod_match == 1) {
    printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);
  } else {
    printf("\nfailed to find a matching tablet config\n");
    exit(EXIT_FAILURE);
  }

  double left = (x - w / 2) * res;
  double right = (x + w / 2) * res;
  double top = (y - h / 2) * res;
  double bottom = (y + h / 2) * res;

  tablet_conf.left = (int)left;
  tablet_conf.right = (int)right;
  tablet_conf.top = (int)top;
  tablet_conf.bottom = (int)bottom;

  printf("\nconverted config\n");
  print_tablet_config(tablet_conf);

  return tablet_conf;
}

struct tablet_config get_tablet_config(int vendor, int product) {
  FILE *conf_file;
  conf_file = fopen("./conf/tablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(1);
  }

  struct tablet_config tablet_conf;
  char line[128] = "";
  bool vend_match = 0;
  bool prod_match = 0;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, " ");

    if (vend_match == 1 && prod_match == 1 && strvalue == NULL)
      break;

    if (line[0] == '[') {
      printf("\nNew conf entry: %s", key);
      vend_match = 0;
      prod_match = 0;
      memset(&tablet_conf, 0, sizeof(tablet_conf));
    }

    if (strvalue != NULL) {
      int value = (int)strtol(strvalue, NULL, 0);

      if (strcmp("left", key) == 0) {
        tablet_conf.left = value;
      } else if (strcmp("right", key) == 0) {
        tablet_conf.right = value;
      } else if (strcmp("top", key) == 0) {
        tablet_conf.top = value;
      } else if (strcmp("bottom", key) == 0) {
        tablet_conf.bottom = value;
      } else if (strcmp("xindex", key) == 0) {
        tablet_conf.xindex = (uint8_t)value;
      } else if (strcmp("yindex", key) == 0) {
        tablet_conf.yindex = (uint8_t)value;
      } else if (strcmp("bindex", key) == 0) {
        tablet_conf.bindex = (uint8_t)value;
      } else if (strcmp("vendor", key) == 0 && value == vendor) {
        vend_match = 1;
      } else if (strcmp("product", key) == 0 && value == product) {
        prod_match = 1;
      }

      printf("%s=%d\n", key, value);
    }
  }

  if (vend_match == 1 && prod_match == 1) {
    printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);
  } else {
    printf("\nfailed to find a matching tablet config\n");
    exit(0);
  }

  fclose(conf_file);

  return tablet_conf;
}

struct tablet_config get_tablet_config_trim(int vendor, int product) {
  FILE *conf_file;
  conf_file = fopen("./conf/tablets.conf", "r");

  if (conf_file == NULL) {
    printf("Err when opening file");
    exit(EXIT_FAILURE);
  }

  struct tablet_config tablet_conf;
  char line[128] = "";
  bool vend_match = 0;
  bool prod_match = 0;

  while ((fgets(line, 128, conf_file)) != NULL) {
    char *key = strtok(line, "=");
    char *strvalue = strtok(NULL, " ");
    char *key_start = key;

    // printf("strvalue:%s\n", strvalue);

    if (vend_match == 1 && prod_match == 1 && strvalue == NULL)
      break;

    // if (key != NULL && strvalue == NULL)
    //   printf("Could not match: %s", key);

    while (isspace(*key_start)) {
      if (key_start == key)
        printf("key_start init:%p\n", key_start);

      key_start++;
    }

    if (key_start != key)
      printf("key_start end:%p\n", key_start);

    // if (isspace(key_start[0]))
    //   printf("isspace\n");

    if (line[0] == '[') {
      printf("\nNew conf entry: %s", key);

      // char *name_start = strstr(line, "[");
      // char *name_end = strstr(line, "]");

      // char *name_start = strtok(key, "[");
      // char *name_end = strtok(name, "]");
      // printf("name:%s, %s\n", name, name2);

      vend_match = 0;
      prod_match = 0;
      memset(&tablet_conf, 0, sizeof(tablet_conf));
    }

    if (strvalue != NULL) {
      int value = (int)strtol(strvalue, NULL, 0);

      // char *ven = "vendor";
      // char *ind = strstr(key, ven); // substr

      if (strcmp("left", key_start) == 0) {
        tablet_conf.left = value;
      } else if (strcmp("right", key_start) == 0) {
        tablet_conf.right = value;
      } else if (strcmp("top", key_start) == 0) {
        tablet_conf.top = value;
      } else if (strcmp("bottom", key_start) == 0) {
        tablet_conf.bottom = value;
      } else if (strcmp("xindex", key) == 0) {
        tablet_conf.xindex = (uint8_t)value;
      } else if (strcmp("yindex", key) == 0) {
        tablet_conf.yindex = (uint8_t)value;
      } else if (strcmp("bindex", key) == 0) {
        tablet_conf.bindex = (uint8_t)value;
      } else if (strcmp("vendor", key_start) == 0 && value == vendor) {
        vend_match = 1;
      } else if (strcmp("product", key_start) == 0 && value == product) {
        prod_match = 1;
      }

      printf("%s=%d\n", key, value);
    }
  }

  if (vend_match == 1 && prod_match == 1) {
    printf("matched vendor: %d, matched product: %d\n", vend_match, prod_match);
  } else {
    printf("\nfailed to find a matching tablet config\n");
    exit(0);
  }

  fclose(conf_file);

  return tablet_conf;
}

// use as possible override for autodetected values
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
