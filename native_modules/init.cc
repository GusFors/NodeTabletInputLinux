#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <linux/input.h>
#include <nan.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include "display.h"

int32_t fd;
int32_t fdn;
bool isClick = false;
int32_t clickValue = 0;
int32_t mBtn = 0;
bool isActive = false;
int32_t xOffset;
int32_t yOffset;
int32_t yPrimaryHeight;
int32_t xPrimaryWidth;

NAN_METHOD(initRead) {
  bool running = false;
  char *device;

  device = (*Nan::Utf8String(info[0]));
  std::cout << "Created uinput device: " << *Nan::Utf8String(info[1]) << "\n";

  fdn = open(device, O_RDONLY | O_SYNC);

  if (fdn < 0) {
    printf("Unable to open device");
    exit(EXIT_FAILURE);
  }
  printf("reading reports from: %s", device);

  // struct hidraw_report_descriptor rd;
  // struct hidraw_devinfo hinf;
  // memset(&rd, 0x0, sizeof(rd));
  // memset(&hinf, 0x0, sizeof(hinf));

  initUinputN(*Nan::Utf8String(info[1]), Nan::To<int32_t>(info[2]).FromJust(), Nan::To<int32_t>(info[3]).FromJust());
  running = true;

  // TODO: read from json config file instead of going through node to send all values
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

  int32_t xBufferPos = Nan::To<int32_t>(info[12]).FromJust();
  int32_t yBufferPos = Nan::To<int32_t>(info[13]).FromJust();

  std::cout << "\n"
            << "xOffset: " << xOffset << " yOffset: " << yOffset;
  std::cout << "\n"
            << "xPrimaryWidth: " << xPrimaryWidth << " yPrimaryHeight: " << yPrimaryHeight;
  std::cout << "\n"
            << "xBufferPos: " << xBufferPos << " yBufferPos: " << yBufferPos;

  int32_t x = 0;
  int32_t y = 0;
  double xS = 0;
  double yS = 0;
  int32_t res;

  int8_t buf[256];
  memset(buf, 0x0, sizeof(buf));

  while (running) {
    res = read(fdn, buf, 16);

    if (res < 0) {
      perror("read err");
      exit(EXIT_FAILURE);
    } else {
      // print_hex_buffer(buf);

      x = (buf[xBufferPos] & 0xff) | ((buf[xBufferPos + 1] & 0xff) << 8);
      y = (buf[yBufferPos] & 0xff) | ((buf[yBufferPos + 1] & 0xff) << 8);

      // x = (buf[2] & 0xff) | ((buf[3] & 0xff) << 8);
      // y = (buf[4] & 0xff) | ((buf[5] & 0xff) << 8);

      xS = (x - left) * xScale;
      yS = (y - top) * yScale;

      if (xS < 0)
        xS = 0;

      if (yS < 0)
        yS = 0;

      if ((buf[0] & 0xff) < 0x11) {
        // std::cout << ((buf[1] & 0xff) & 0x07) << "\n";
        setUinputPointerN(xS, yS, 0, buf[1]);
      }

      // if ((buf[0] & 0xff) < 0x10) {
      //   // std::cout << ((buf[1] & 0xff) & 0x07) << "\n";
      //   setUinputPointerN(xS, yS, 0, buf[1]);
      // }
    }
  }
}

NAN_MODULE_INIT(init) { Nan::SetMethod(target, "initRead", initRead); }

NODE_MODULE(pointerN, init);