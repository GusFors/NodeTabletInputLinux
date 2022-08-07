#include <cstdio>
#include <iostream>
#include <nan.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <nan.h>
#include <nan_converters.h>
#include <node.h>

Display *display = NULL;
Window root = 0;
int fd;
struct uinput_user_dev uiPointer;
// struct uinput_abs_setup uiPressure;

// formatter might change include order, place nan.h first after formatting or
// rebuild might fail

NAN_METHOD(setPointer) {
  // scaled x and y values from node
  int32_t x = Nan::To<int32_t>(info[0]).FromJust();
  int32_t y = Nan::To<int32_t>(info[1]).FromJust();

  // set absolute position
  XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
  // XSync(display, true);

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

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

  ioctl(fd, UI_SET_EVBIT, EV_ABS);
  ioctl(fd, UI_SET_ABSBIT, ABS_X);
  ioctl(fd, UI_SET_ABSBIT, ABS_Y);

  if (isPressure) {
    std::string pStr = (isPressure) ? "true" : "false";
    std::cout << "Pressure: " << pStr << "\n";
    ioctl(fd, UI_SET_ABSBIT, ABS_PRESSURE);
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
    // uiPressure.absinfo.minimum = 0;
    // uiPressure.absinfo.maximum = 1023;
  }

  write(fd, &uiPointer, sizeof(uiPointer));
  ioctl(fd, UI_DEV_CREATE);

  info.GetReturnValue().Set(Nan::New(devName).ToLocalChecked());
}

NAN_METHOD(setUinputPointer) {
  int32_t x = Nan::To<int32_t>(info[0]).FromJust();
  int32_t y = Nan::To<int32_t>(info[1]).FromJust();

  struct input_event positionEvents[2];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_ABS;
  positionEvents[0].code = ABS_X;
  positionEvents[0].value = x;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  positionEvents[1].type = EV_ABS;
  positionEvents[1].code = ABS_Y;
  positionEvents[1].value = y;
  positionEvents[1].time.tv_sec = 0;
  positionEvents[1].time.tv_usec = 0;

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uPressure) {

  int32_t pressure = Nan::To<int32_t>(info[0]).FromJust();

  std::cout << pressure << "\n";

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_ABS;
  positionEvents[0].code = ABS_PRESSURE;
  positionEvents[0].value = pressure;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseLeftClickDown) {

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_LEFT;
  positionEvents[0].value = 1;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

  struct input_event syncEvent;
  memset(&syncEvent, 0, sizeof(syncEvent));

  syncEvent.type = EV_SYN;
  syncEvent.value = 0;
  syncEvent.code = 0;
  write(fd, &syncEvent, sizeof(syncEvent));
}

NAN_METHOD(uMouseLeftClickUp) {

  struct input_event positionEvents[1];
  memset(positionEvents, 0, sizeof(positionEvents));

  positionEvents[0].type = EV_KEY;
  positionEvents[0].code = BTN_LEFT;
  positionEvents[0].value = 0;
  positionEvents[0].time.tv_sec = 0;
  positionEvents[0].time.tv_usec = 0;

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

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

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

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

  int res_w = write(fd, positionEvents, sizeof(positionEvents));

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

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "setPointer", setPointer);
  Nan::SetMethod(target, "initDisplay", initDisplay);
  Nan::SetMethod(target, "initUinput", initUinput);
  Nan::SetMethod(target, "setUinputPointer", setUinputPointer);
  Nan::SetMethod(target, "setMotionEventPointer", setMotionEventPointer);
  Nan::SetMethod(target, "mouseLeftClickDown", mouseLeftClickDown);
  Nan::SetMethod(target, "mouseLeftClickUp", mouseLeftClickUp);
  Nan::SetMethod(target, "uPressure", uPressure);
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