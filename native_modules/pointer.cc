#include <nan.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

using namespace v8;

Display *display = NULL;
Window root = NULL;

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

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "setPointer", setPointer);
  Nan::SetMethod(target, "initDisplay", initDisplay);
}

NODE_MODULE(pointer, init);