#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>
#include "display.h"

// Display *display = NULL;

int get_displays_total_width(Display *display) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int display_width = XDisplayWidth(display, 0);

  return display_width;
}

int get_displays_total_height(Display *display) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int display_height = XDisplayHeight(display, 0);

  return display_height;
}

int get_primary_monitor_xoffset(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  return mon_info->x;
}

int get_primary_monitor_yoffset(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  // XRRFreeCrtcInfo(mon_info);
  return mon_info->y;
}

int get_primary_monitor_width(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  return mon_info->width;
}

int get_primary_monitor_height(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  return mon_info->height;
}

int get_number_of_monitors(Display *display) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int num_monitors = 0;
  XRRGetMonitors(display, XDefaultRootWindow(display), 1, &num_monitors);

  return num_monitors;
}

struct display_config get_display_config() {
  struct display_config conf;
  Display *display = XOpenDisplay(NULL);
  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  return conf;
}

int close_display(Display *display) { return XCloseDisplay(display); }

void free_xresources(XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  XRRFreeScreenResources(mon_res);
  XRRFreeCrtcInfo(mon_info);
}
