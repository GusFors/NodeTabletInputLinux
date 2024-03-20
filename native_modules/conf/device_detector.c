#include "device_detector.h"
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

struct device_info detect_tablet() {
  DIR *hidraw_dir;
  struct dirent *hid_entry;
  struct device_info tablet;
  memset(&tablet, 0, sizeof(tablet));

  hidraw_dir = opendir("/sys/class/hidraw");

  if (hidraw_dir == NULL) {
    printf("error opening dir\n");
    exit(0);
  }

  int found_device = 0;

  while ((hid_entry = readdir(hidraw_dir)) != NULL) {
    if (hid_entry->d_type == DT_LNK) {
      char catpath[256] = "/sys/class/hidraw/";
      strlcat(catpath, hid_entry->d_name, 256);
      strlcat(catpath, "/device/uevent", 256);

      FILE *uevent_file;
      uevent_file = fopen(catpath, "r");

      char line[128] = "";
      int vendor = 0;
      int product = 0;

      while ((fgets(line, 128, uevent_file)) != NULL) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");
        // printf("key:%s, value:%s", key, value);

        if (strcmp(key, "HID_ID") == 0) {
          char *valueid_split;
          valueid_split = strtok(value, ":");

          while (valueid_split != NULL) {
            int conv = strtol(valueid_split, NULL, 16);
            if (conv == 1386) {
              vendor = conv;
              valueid_split = strtok(NULL, ":");
              product = strtol(valueid_split, NULL, 16);
              found_device = 1;
            }
            valueid_split = strtok(NULL, ":");
          }
        }

        if (vendor == 1386) {
          // printf("vendor: %d, product: %d, path: %s", vendor, product, hid_entry->d_name);
          tablet.vendor = vendor;
          tablet.product = product;

          char path[128] = "/dev/";
          strlcat(path, hid_entry->d_name, 128);
          strlcpy(tablet.hidraw_path, path, 128);
          break;
        }
      }
      // printf("\n");
      fclose(uevent_file);
    }

    if (found_device)
      break;
  }

  closedir(hidraw_dir);

  if (tablet.product == 0) {
    printf("failed to find tablet hidraw\n");
    exit(0);
  }

  return tablet;
}