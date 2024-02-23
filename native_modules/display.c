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

int get_number_of_monitors(Display *display) {
  if (display == NULL) {
    display = XOpenDisplay(NULL);
  }

  int num_monitors = 0;
  XRRGetMonitors(display, XDefaultRootWindow(display), 1, &num_monitors);

  return num_monitors;
}

int get_primary_monitor_xoffset(XRRCrtcInfo *mon_info) { return mon_info->x; }

int get_primary_monitor_yoffset(XRRCrtcInfo *mon_info) { return mon_info->y; }

int get_primary_monitor_width(XRRCrtcInfo *mon_info) { return mon_info->width; }

int get_primary_monitor_height(XRRCrtcInfo *mon_info) { return mon_info->height; }

void get_full_display_config(struct display_config *display_conf) {
  Display *display = XOpenDisplay(NULL);
  XRRScreenResources *mon_res = XRRGetScreenResources(display, XDefaultRootWindow(display));
  XRRCrtcInfo *mon_info = XRRGetCrtcInfo(display, mon_res, mon_res->crtcs[0]);

  display_conf->total_width = get_displays_total_width(display);
  display_conf->total_height = get_displays_total_height(display);
  display_conf->offset_x = get_primary_monitor_xoffset(mon_info);
  display_conf->offset_y = get_primary_monitor_yoffset(mon_info);
  display_conf->primary_height = get_primary_monitor_height(mon_info);
  display_conf->primary_width = get_primary_monitor_width(mon_info);

  free_xresources(mon_res, mon_info);
  close_display(display);
}

void print_display_config(struct display_config *display_conf) {}

int close_display(Display *display) { return XCloseDisplay(display); }

void free_xresources(XRRScreenResources *mon_res, XRRCrtcInfo *mon_info) {
  XRRFreeScreenResources(mon_res);
  XRRFreeCrtcInfo(mon_info);
}
