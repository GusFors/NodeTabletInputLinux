#include <nan.h>
#include "display.h"

// place nan.h first if rebuild fails
Display *display = XOpenDisplay(NULL);
XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

NAN_METHOD(getDisplaysTotalWidth) {
  int displayWidth = get_displays_total_width(display);
  // DefaultScreen(display);
  // int xWidth = XWidthOfScreen(ScreenOfDisplay(display, Scr));
  // XCloseDisplay(display);
  info.GetReturnValue().Set(Nan::New(displayWidth));
}

NAN_METHOD(getDisplaysTotalHeight) {
  int displayHeight = get_displays_total_height(display);
  info.GetReturnValue().Set(Nan::New(displayHeight));
}

NAN_METHOD(getPrimaryMonitorXoffset) {
  int xoffset = get_primary_monitor_xoffset(mon_info);
  info.GetReturnValue().Set(Nan::New(xoffset));
}

NAN_METHOD(getPrimaryMonitorYoffset) {
  int yoffset = get_primary_monitor_yoffset(mon_info);
  info.GetReturnValue().Set(Nan::New(yoffset));
}

NAN_METHOD(getPrimaryMonitorWidth) {
  int width = get_primary_monitor_width(mon_info);
  info.GetReturnValue().Set(Nan::New(width));
}

NAN_METHOD(getPrimaryMonitorHeight) {
  int height = get_primary_monitor_height(mon_info);
  info.GetReturnValue().Set(Nan::New(height));
}

NAN_METHOD(getNumberOfMonitors) {
  int numMonitors = get_number_of_monitors(display);
  info.GetReturnValue().Set(Nan::New(numMonitors));
}

NAN_METHOD(closeDisplay) {
  int v = close_display(display);
  info.GetReturnValue().Set(Nan::New(v));
}

NAN_METHOD(freeXres) {
  XRRFreeScreenResources(mon_res);
  XRRFreeCrtcInfo(mon_info);
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
  Nan::SetMethod(target, "freeXres", freeXres);
}

NODE_MODULE(display, init);