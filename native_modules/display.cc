#include <nan.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>

Display *display = NULL;
// place nan.h first if rebuild fails

int get_displays_total_width() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int displayWidth = XDisplayWidth(display, 0);

  return displayWidth;
}

int get_displays_total_height() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int displayHeight = XDisplayHeight(display, 0);

  return displayHeight;
}

int get_primary_monitor_xoffset() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  return monInfo->x;
}

int get_primary_monitor_yoffset() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  return monInfo->y;
}

int get_primary_monitor_width() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  return monInfo->width;
}

int get_primary_monitor_height() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  return monInfo->height;
}

int get_number_of_monitors() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int numMonitors = 0;
  XRRGetMonitors(display, XDefaultRootWindow(display), 1, &numMonitors);

  return numMonitors;
}

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

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "getDisplaysTotalWidth", getDisplaysTotalWidth);
  Nan::SetMethod(target, "getDisplaysTotalHeight", getDisplaysTotalHeight);
  Nan::SetMethod(target, "getPrimaryMonitorXoffset", getPrimaryMonitorXoffset);
  Nan::SetMethod(target, "getPrimaryMonitorYoffset", getPrimaryMonitorYoffset);
  Nan::SetMethod(target, "getPrimaryMonitorWidth", getPrimaryMonitorWidth);
  Nan::SetMethod(target, "getPrimaryMonitorHeight", getPrimaryMonitorHeight);
  Nan::SetMethod(target, "getNumberOfMonitors", getNumberOfMonitors);
}

NODE_MODULE(display, init);