#include <nan.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>

using namespace v8;

Display *display = NULL;

// TODO fix an actual formatter for c++, place nan.h first after formatting or
// rebuild might fail
NAN_METHOD(getNumberOfMonitors) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int numMonitors = 0;
  XRRGetMonitors(display, XDefaultRootWindow(display), 1, &numMonitors);

  info.GetReturnValue().Set(Nan::New(numMonitors));
}

NAN_METHOD(getDisplaysTotalWidth) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int x = 0;
  int displayWidth = XDisplayWidth(display, x);

  info.GetReturnValue().Set(Nan::New(displayWidth));
}

NAN_METHOD(getPrimaryMonitorXoffset) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes =
      XRRGetScreenResources(display, XDefaultRootWindow(display));

  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  info.GetReturnValue().Set(Nan::New(monInfo->x));
}

NAN_METHOD(getPrimaryMonitorYoffset) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes =
      XRRGetScreenResources(display, XDefaultRootWindow(display));

  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  info.GetReturnValue().Set(Nan::New(monInfo->y));
}

NAN_METHOD(getPrimaryMonitorWidth) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes =
      XRRGetScreenResources(display, XDefaultRootWindow(display));

  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  info.GetReturnValue().Set(Nan::New(monInfo->width));
}

NAN_METHOD(getPrimaryMonitorHeight) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes =
      XRRGetScreenResources(display, XDefaultRootWindow(display));

  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);

  info.GetReturnValue().Set(Nan::New(monInfo->height));
}

// expose as a node module
NAN_MODULE_INIT(init) {
  Nan::SetMethod(target, "getDisplaysTotalWidth", getDisplaysTotalWidth);
  Nan::SetMethod(target, "getNumberOfMonitors", getNumberOfMonitors);
  Nan::SetMethod(target, "getPrimaryMonitorXoffset", getPrimaryMonitorXoffset);
  Nan::SetMethod(target, "getPrimaryMonitorYoffset", getPrimaryMonitorYoffset);
  Nan::SetMethod(target, "getPrimaryMonitorWidth", getPrimaryMonitorWidth);
  Nan::SetMethod(target, "getPrimaryMonitorHeight", getPrimaryMonitorHeight);
}

NODE_MODULE(display, init);