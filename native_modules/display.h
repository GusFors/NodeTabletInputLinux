#ifndef DISPLAY_H
#define DISPLAY_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

struct display_config {
  int total_width;
  int total_height;
  int offset_x;
  int offset_y;
  int primary_height;
  int primary_width;
};

#ifdef __cplusplus
extern "C" {
#endif

int get_displays_total_width(Display *display);
int get_displays_total_height(Display *display);
int get_primary_monitor_xoffset(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);
int get_primary_monitor_yoffset(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);
int get_primary_monitor_width(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);
int get_primary_monitor_height(Display *display, XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);
int get_number_of_monitors(Display *display);
int close_display(Display *display);
void free_xresources(XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);

#ifdef __cplusplus
}
#endif

#endif