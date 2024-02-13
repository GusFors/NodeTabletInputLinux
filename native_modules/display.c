#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>
#include "display.h"

Display *display = NULL;

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

int close_display() { return XCloseDisplay(display); }
