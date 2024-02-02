#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/input.h>
#include <nan.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include "display.h"
#include "tablet.h"

NAN_METHOD(initRead) {
  int32_t xOffset;
  int32_t yOffset;
  int32_t yPrimaryHeight;
  int32_t xPrimaryWidth;

  // char *hidraw_path;
  std::string hidraw_path = (*Nan::Utf8String(info[0]));
  std::string device_name = *Nan::Utf8String(info[1]);
  int32_t xMax = Nan::To<int32_t>(info[2]).FromJust();
  int32_t yMax = Nan::To<int32_t>(info[3]).FromJust();

  init_uinput(("Virtual uinput " + device_name).c_str(), xMax, yMax);
  std::cout << "Created device: " << "Virtual uinput " + device_name;

  // read from json config file instead of going through node to send all values
  int32_t left = Nan::To<int32_t>(info[4]).FromJust();
  int32_t top = Nan::To<int32_t>(info[5]).FromJust();
  double xScale = Nan::To<double>(info[6]).FromJust();
  double yScale = Nan::To<double>(info[7]).FromJust();

  xOffset = get_primary_monitor_xoffset();
  yOffset = get_primary_monitor_yoffset();
  xPrimaryWidth = get_primary_monitor_width();
  yPrimaryHeight = get_primary_monitor_height();
  int v = close_display();

  // xOffset = Nan::To<int32_t>(info[8]).FromJust();
  // yOffset = Nan::To<int32_t>(info[9]).FromJust();
  // xPrimaryWidth = Nan::To<int32_t>(info[10]).FromJust();
  // yPrimaryHeight = Nan::To<int32_t>(info[11]).FromJust();

  int32_t xBufferPos = Nan::To<int32_t>(info[8]).FromJust();
  int32_t yBufferPos = Nan::To<int32_t>(info[9]).FromJust();

  struct tablet_config tablet;
  tablet.left = left;
  tablet.top = top;
  tablet.xscale = xScale;
  tablet.yscale = yScale;
  tablet.xindex = xBufferPos;
  tablet.yindex = yBufferPos;
  // tablet.hidraw_path = tablet_path;

  struct display_config display_conf;
  display_conf.xoffset = xOffset;
  display_conf.yoffset = yOffset;
  display_conf.primary_height = yPrimaryHeight;
  display_conf.primary_width = xPrimaryWidth;

  std::cout << "\n"
            << "xOffset: " << xOffset << " yOffset: " << yOffset;
  std::cout << "\n"
            << "xPrimaryWidth: " << xPrimaryWidth << " yPrimaryHeight: " << yPrimaryHeight;
  std::cout << "\n"
            << "xBufferPos: " << xBufferPos << " yBufferPos: " << yBufferPos << "\n";

  init_read(tablet, display_conf, hidraw_path.c_str());

  // parse_tablet_buffer(tablet);
}

NAN_MODULE_INIT(init) { Nan::SetMethod(target, "initRead", initRead); }

NODE_MODULE(tablet, init);