#ifndef DEVICE_DETECTOR_H
#define DEVICE_DETECTOR_H

#include "../tablet.h"
#include <stdint.h>
#include <stdbool.h>

struct device_info {
  int16_t vendor;
  int16_t product;
  char hidraw_path[128];
};

#ifdef __cplusplus
extern "C" {
#endif

struct device_info detect_tablet(bool verbose);

#ifdef __cplusplus
}
#endif

#endif
