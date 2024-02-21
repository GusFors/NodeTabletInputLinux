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

  int display_width = XDisplayWidth(display, 0);

  return display_width;
}

int get_displays_total_height() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int display_height = XDisplayHeight(display, 0);

  return display_height;
}

int get_primary_monitor_xoffset() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  return mon_info->x;
}

int get_primary_monitor_yoffset() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  return mon_info->y;
}

int get_primary_monitor_width() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  return mon_info->width;
}

int get_primary_monitor_height() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  return mon_info->height;
}

int get_number_of_monitors() {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int num_monitors = 0;
  XRRGetMonitors(display, XDefaultRootWindow(display), 1, &num_monitors);

  return num_monitors;
}

int close_display() { return XCloseDisplay(display); }
