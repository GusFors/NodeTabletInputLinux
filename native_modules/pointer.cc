#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <nan.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>

Display *display = NULL;
Window root = 0;
int32_t fd;
struct uinput_user_dev uiPointer;
struct uinput_abs_setup uiPressure;
struct input_absinfo uInfo;

// formatter might change include order, place nan.h first after formatting or
// rebuild might fail

NAN_METHOD(setPointer) {
  // scaled x and y values from node
  int32_t x = Nan::To<int32_t>(info[0]).FromJust();
  int32_t y = Nan::To<int32_t>(info[1]).FromJust();

  // set absolute position
  XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);

  XFlush(display);
  info.GetReturnValue().Set(Nan::New(1));
}

NAN_METHOD(initDisplay) {
  // set display and root window
  if (display == NULL) {
    display = XOpenDisplay(":0.0");
    root = XDefaultRootWindow(display);
    info.GetReturnValue().Set(Nan::New(1));
  }
}

NAN_METHOD(initUinput) {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  std::string devName = "Virtual uinput " + std::string(*Nan::Utf8String(info[0]));
  int32_t xMax = Nan::To<int32_t>(info[1]).FromJust();
  int32_t yMax = Nan::To<int32_t>(info[2]).FromJust();
  bool isPressure = Nan::To<bool>(info[3]).FromJust();

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

  ioctl(fd, UI_SET_EVBIT, EV_ABS);
  ioctl(fd, UI_SET_ABSBIT, ABS_X);
  ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  std::string pStr = (isPressure) ? "true" : "false";
  std::cout << "Pressure: " << pStr << "\n";
  if (isPressure) {
    ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE);
    // ioctl(fd, UI_SET_KEYBIT, BTN_TOUCH);
    // ioctl(fd, UI_SET_ABSBIT, BTN_STYLUS);
    // ioctl(fd, UI_SET_ABSBIT, BTN_TOOL_PEN); // keybit or absbit?
  }

  // ioctl(fd, UI_SET_EVBIT, EV_REL);

  memset(&uiPointer, 0, sizeof(uiPointer));
  snprintf(uiPointer.name, UINPUT_MAX_NAME_SIZE, "%s", devName.c_str());

  uiPointer.id.bustype = BUS_USB;
  uiPointer.id.version = 1;
  uiPointer.id.vendor = 0x1;
  uiPointer.id.product = 0x1;

  uiPointer.absmin[ABS_X] = 0;
  uiPointer.absmax[ABS_X] = xMax;

  uiPointer.absmin[ABS_Y] = 0;
  uiPointer.absmax[ABS_Y] = yMax;

  if (isPressure) {
    uiPointer.absmin[ABS_PRESSURE] = 0;
    uiPointer.absmax[ABS_PRESSURE] = 1023;
    uiPointer.absflat[ABS_Y] = 0;
    uiPointer.absfuzz[ABS_Y] = 0;
    uiPointer.absflat[ABS_X] = 0;
    uiPointer.absfuzz[ABS_X] = 0;
  }

  write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);

  info.GetReturnValue().Set(Nan::New(devName).ToLocalChecked());
}

NAN_METHOD(setUinputPointer) {
  int32_t x = Nan::To<int32_t>(info[0]).FromJust();
  int32_t y = Nan::To<int32_t>(info[1]).FromJust();
  int32_t pressure = Nan::To<int32_t>(info[2]).FromJust();
  int32_t mouseClick = Nan::To<int32_t>(info[3]).FromJust();
  float pressurePercentage = pressure / 1023.0;

  std::cout << "pressure:" << pressure << ", " << pressurePercentage * 100 << "%\n";
  // std::cout << "mouseclick:" << mouseClick << "\n";

  struct input_event positionEvents[info.Length() + 1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_TOOL_PEN;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  positionEvents[1].type = EV_ABS;
  positionEvents[1].code = ABS_X;
  positionEvents[1].value = x;
  positionEvents[1].time.tv_sec = 0;
  positionEvents[1].time.tv_usec = 0;

  positionEvents[2].type = EV_ABS;
  positionEvents[2].code = ABS_Y;
  positionEvents[2].value = y;
  positionEvents[2].time.tv_sec = 0;
  positionEvents[2].time.tv_usec = 0;

  positionEvents[3].type = EV_ABS;
  positionEvents[3].code = ABS_PRESSURE;
  positionEvents[3].value = pressure;
  positionEvents[3].time.tv_sec = 0;
  positionEvents[3].time.tv_usec = 0;

  // switch (mouseClick) {
  // case 0x01:
  //   if (!isClick) {
  //     isClick = true;
  //   }
  // }

  if (mouseClick > -1) {
    positionEvents[3].type = EV_KEY;
    positionEvents[3].code = BTN_LEFT;
    positionEvents[3].value = mouseClick;
    positionEvents[3].time.tv_sec = 0;
    positionEvents[3].time.tv_usec = 0;
    //  std::cout << "mouseclick:" << mouseClick << "\n";
  }

  // if (mouseClick == 1) {
  //   std::cout << "leftdown"
  //             << "\n";
  //   positionEvents[4].type = EV_KEY;
  //   positionEvents[4].code = BTN_LEFT;
  //   positionEvents[4].value = 1;
  //   positionEvents[4].time.tv_sec = 0;
  //   positionEvents[4].time.tv_usec = 0;
  // } else if (mouseClick == 0) {
  //   std::cout << "leftup"
  //             << "\n";
  //   positionEvents[4].type = EV_KEY;
  //   positionEvents[4].code = BTN_LEFT;
  //   positionEvents[4].value = 0;
  //   positionEvents[4].time.tv_sec = 0;
  //   positionEvents[4].time.tv_usec = 0;
  // }

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent)); // opt?

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(setUPressurePointer) {

  int32_t pressure = Nan::To<int32_t>(info[0]).FromJust();

  std::cout << pressure << "\n";

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_ABS;
  positionEvents[0].code = ABS_PRESSURE;
  positionEvents[0].value = pressure;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseLeftClickDown) {

  std::cout << "leftdown";

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_LEFT;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseLeftClickUp) {

  std::cout << "leftup";

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_LEFT;
  positionEvents[0].value = 0;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseRightClickDown) {

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_RIGHT;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseRightClickUp) {

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_RIGHT;
  positionEvents[0].value = 0;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(setMotionEventPointer) {
  int32_t x = Nan::To<int32_t>(info[0]).FromJust();
  int32_t y = Nan::To<int32_t>(info[1]).FromJust();

  XTestFakeMotionEvent(display, 0, x, y, CurrentTime);
  XFlush(display);

  info.GetReturnValue().Set(Nan::New(1));
}

NAN_METHOD(mouseLeftClickDown) {
  // test mouse click down
  XTestFakeButtonEvent(display, 1, true, CurrentTime);
  XFlush(display);
}

NAN_METHOD(mouseLeftClickUp) {
  // test mouse click
  XTestFakeButtonEvent(display, 1, false, CurrentTime);
  XFlush(display);
}

NAN_METHOD(mouseRightClickDown) {
  XTestFakeButtonEvent(display, 3, true, CurrentTime);
  XFlush(display);
}

NAN_METHOD(mouseRightClickUp) {
  XTestFakeButtonEvent(display, 3, false, CurrentTime);
  XFlush(display);
}

void initUinputN(std::string name, int32_t xMax, int32_t yMax) {
  fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  std::string devName = "Virtual uinput " + std::string(name);

  ioctl(fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);
  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

  ioctl(fd, UI_SET_EVBIT, EV_ABS);
  ioctl(fd, UI_SET_ABSBIT, ABS_X);
  ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  memset(&uiPointer, 0, sizeof(uiPointer));
  snprintf(uiPointer.name, UINPUT_MAX_NAME_SIZE, "%s", devName.c_str());

  uiPointer.id.bustype = BUS_USB;
  uiPointer.id.version = 1;
  uiPointer.id.vendor = 0x1;
  uiPointer.id.product = 0x1;

  uiPointer.absmin[ABS_X] = 0;
  uiPointer.absmax[ABS_X] = xMax;

  uiPointer.absmin[ABS_Y] = 0;
  uiPointer.absmax[ABS_Y] = yMax;

  write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);
}

bool isClick = false;
int32_t clickValue = 0;
int32_t mBtn = 0;
bool isActive = false;

void setUinputPointerN(int32_t x, int32_t y, int32_t pressure, int32_t btn) {

  if (x > 2560)
    x = 2560;

  if (x < 0)
    x = 0;

  if (y < 0)
    y = 0;

  if (y > 1440)
    y = 1440;

  if (x == 0 && y == 0) {
    return;
  }

  // std::cout << "x: " << x << " y: " << y << "\n";

  struct input_event positionEvents[4];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_TOOL_PEN;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  positionEvents[1].type = EV_ABS;
  positionEvents[1].code = ABS_X;
  positionEvents[1].value = x + 2560;
  positionEvents[1].time.tv_sec = 0;
  positionEvents[1].time.tv_usec = 0;

  positionEvents[2].type = EV_ABS;
  positionEvents[2].code = ABS_Y;
  positionEvents[2].value = y;
  positionEvents[2].time.tv_sec = 0;
  positionEvents[2].time.tv_usec = 0;

  // std::cout << ((btn & 0xff) & 0x07);

  switch (((btn & 0xff) & 0x07)) {
  case 0x01:
    clickValue = 1;
    if (isClick == false) {
      mBtn = BTN_LEFT;
      // std::cout << mBtn << "down\n";
      isClick = true;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
    }
    break;

  case 0x04:
    clickValue = 1;
    if (isClick == false) {
      mBtn = BTN_RIGHT;
      // std::cout << mBtn << "down\n";
      isClick = true;
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
    }
    break;

  case 0x00: // elr default?
    clickValue = 0;
    if (isClick) {
      isClick = false;
      // std::cout << mBtn << " up\n";
      positionEvents[3].type = EV_KEY;
      positionEvents[3].code = mBtn;
      positionEvents[3].value = clickValue;
      positionEvents[3].time.tv_sec = 0;
      positionEvents[3].time.tv_usec = 0;
      mBtn = 0;
    }
  }

  int32_t res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent)); // opt?

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

int32_t fdn;
int32_t i;
int32_t res;
int32_t desc_size = 0;

int8_t buf[256];
struct hidraw_report_descriptor rd;
struct hidraw_devinfo hinf;
char *device;
bool running = 0;

NAN_METHOD(initRead) {
  device = (*Nan::Utf8String(info[0]));
  std::cout << "Created uinput device:" << *Nan::Utf8String(info[1]);

  fdn = open(device, O_RDONLY | O_SYNC);

  if (fdn < 0) {
    perror("Unable to open device");
  }

  memset(&rd, 0x0, sizeof(rd));
  memset(&hinf, 0x0, sizeof(info));
  memset(buf, 0x0, sizeof(buf));

  running = true;
  initUinputN(*Nan::Utf8String(info[1]), Nan::To<int32_t>(info[2]).FromJust(), Nan::To<int32_t>(info[3]).FromJust());

  int32_t left = Nan::To<int32_t>(info[4]).FromJust();
  int32_t top = Nan::To<int32_t>(info[5]).FromJust();
  double xScale = Nan::To<double>(info[6]).FromJust();
  double yScale = Nan::To<double>(info[7]).FromJust();

  int32_t x = 0;
  int32_t y = 0;
  double xS = 0;
  double yS = 0;

  while (running) {
    res = read(fdn, buf, 16);

    if (res < 0) {
      perror("read err");
      std::exit(EXIT_FAILURE);
    } else {

      x = (buf[2] & 0xff) | ((buf[3] & 0xff) << 8);
      y = (buf[4] & 0xff) | ((buf[5] & 0xff) << 8);

      xS = (x - left) * xScale;
      yS = (y - top) * yScale;

      if (xS < 0)
        xS = 0;

      if (yS < 0)
        yS = 0;

      if ((buf[0] & 0xff) < 0x10) {
        // std::cout << ((buf[1] & 0xff) & 0x07) << "\n";
        setUinputPointerN(xS, yS, 0, buf[1]);
      }
    }
  }
}

void readDeviceN() {
  res = read(fdn, buf, 16);

  int32_t x = 0;
  if (res < 0) {
    perror("read err");
  } else {
    x = buf[2] | (buf[3] << 8);
    for (i = 0; i < res; i++) {
      printf("%hhx ", buf[i]);
    }
  }
};

NAN_METHOD(readDevice) {
  res = read(fdn, buf, 16);

  int32_t x = 0;
  if (res < 0) {
    perror("read err");
  } else {

    x = buf[2] | (buf[3] << 8);
    for (i = 0; i < res; i++) {
    }
  }

  info.GetReturnValue().Set(x);
}

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "initRead", initRead);
  Nan::SetMethod(target, "readDevice", readDevice);
  Nan::SetMethod(target, "setPointer", setPointer);
  Nan::SetMethod(target, "initDisplay", initDisplay);
  Nan::SetMethod(target, "initUinput", initUinput);
  Nan::SetMethod(target, "setUinputPointer", setUinputPointer);
  Nan::SetMethod(target, "setMotionEventPointer", setMotionEventPointer);
  Nan::SetMethod(target, "mouseLeftClickDown", mouseLeftClickDown);
  Nan::SetMethod(target, "mouseLeftClickUp", mouseLeftClickUp);
  Nan::SetMethod(target, "setUPressurePointer", setUPressurePointer);
  Nan::SetMethod(target, "uMouseLeftClickDown", uMouseLeftClickDown);
  Nan::SetMethod(target, "uMouseLeftClickUp", uMouseLeftClickUp);
  Nan::SetMethod(target, "uMouseRightClickDown", uMouseRightClickDown);
  Nan::SetMethod(target, "uMouseRightClickUp", uMouseRightClickUp);
  Nan::SetMethod(target, "mouseRightClickDown", mouseRightClickDown);
  Nan::SetMethod(target, "mouseRightClickUp", mouseRightClickUp);
}

NODE_MODULE(pointer, init);

void cvstr(std::string str) {
  char chcv[str.length() + 1];
  for (int x = 0; x < sizeof(chcv); x++) {
    chcv[x] = str[x];
    std::cout << chcv[x];
  }
}