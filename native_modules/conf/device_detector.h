#ifndef DEVICE_DETECTOR_H
#define DEVICE_DETECTOR_H
#include "../tablet.h"

struct device_info {
  int vendor;
  int product;
  char hidraw_path[128];
};

#ifdef __cplusplus
extern "C" {
#endif

struct device_info detect_tablet();

#ifdef __cplusplus
}
#endif

#endif