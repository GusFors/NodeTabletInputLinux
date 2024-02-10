#include <nan.h>
#include "display.h"

// place nan.h first if rebuild fails

NAN_METHOD(getDisplaysTotalWidth) {
  int displayWidth = get_displays_total_width();
  // auto Scr = DefaultScreen(display);
  // int xWidth = XWidthOfScreen(ScreenOfDisplay(display, Scr));
  // XCloseDisplay(display);
  info.GetReturnValue().Set(Nan::New(displayWidth));
}

NAN_METHOD(getDisplaysTotalHeight) {
  int displayHeight = get_displays_total_height();
  info.GetReturnValue().Set(Nan::New(displayHeight));
}

NAN_METHOD(getPrimaryMonitorXoffset) {
  int xoffset = get_primary_monitor_xoffset();
  info.GetReturnValue().Set(Nan::New(xoffset));
}

NAN_METHOD(getPrimaryMonitorYoffset) {
  int yoffset = get_primary_monitor_yoffset();
  info.GetReturnValue().Set(Nan::New(yoffset));
}

NAN_METHOD(getPrimaryMonitorWidth) {
  int width = get_primary_monitor_width();
  info.GetReturnValue().Set(Nan::New(width));
}

NAN_METHOD(getPrimaryMonitorHeight) {
  int height = get_primary_monitor_height();
  info.GetReturnValue().Set(Nan::New(height));
}

NAN_METHOD(getNumberOfMonitors) {
  int numMonitors = get_number_of_monitors();
  info.GetReturnValue().Set(Nan::New(numMonitors));
}

NAN_METHOD(closeDisplay) {
  int v = close_display();
  info.GetReturnValue().Set(Nan::New(v));
}

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "getDisplaysTotalWidth", getDisplaysTotalWidth);
  Nan::SetMethod(target, "getDisplaysTotalHeight", getDisplaysTotalHeight);
  Nan::SetMethod(target, "getPrimaryMonitorXoffset", getPrimaryMonitorXoffset);
  Nan::SetMethod(target, "getPrimaryMonitorYoffset", getPrimaryMonitorYoffset);
  Nan::SetMethod(target, "getPrimaryMonitorWidth", getPrimaryMonitorWidth);
  Nan::SetMethod(target, "getPrimaryMonitorHeight", getPrimaryMonitorHeight);
  Nan::SetMethod(target, "getNumberOfMonitors", getNumberOfMonitors);
  Nan::SetMethod(target, "closeDisplay", closeDisplay);
}

NODE_MODULE(display, init);