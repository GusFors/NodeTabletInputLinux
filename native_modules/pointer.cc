#include <nan.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>

using namespace v8;
using namespace Nan;

Display *display = NULL;
Window root = 0;

// TODO fix an actual formatter for c++, place nan.h first after formatting or
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
  Nan::SetMethod(target, "mouseLeftClickDown", mouseLeftClickDown);
  Nan::SetMethod(target, "mouseLeftClickUp", mouseLeftClickUp);
  Nan::SetMethod(target, "mouseRightClickDown", mouseRightClickDown);
  Nan::SetMethod(target, "mouseRightClickUp", mouseRightClickUp);
}

NODE_MODULE(pointer, init);