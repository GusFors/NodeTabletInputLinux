#ifndef DISPLAY_H
#define DISPLAY_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

struct display_config {
  unsigned int primary_height;
  unsigned int primary_width;
  int total_width;
  int total_height;
  int offset_x;
  int offset_y;
};

#ifdef __cplusplus
extern "C" {
#endif

int get_displays_total_width(Display *display);
int get_displays_total_height(Display *display);
int get_number_of_monitors(Display *display);
int get_primary_monitor_xoffset(XRRCrtcInfo *mon_info);
int get_primary_monitor_yoffset(XRRCrtcInfo *mon_info);
unsigned int get_primary_monitor_width(XRRCrtcInfo *mon_info);
unsigned int get_primary_monitor_height(XRRCrtcInfo *mon_info);

int close_display(Display *display);
void free_xresources(XRRScreenResources *mon_res, XRRCrtcInfo *mon_info);
void get_full_display_config(struct display_config *display_conf);
void print_display_config(struct display_config *display_conf);
// struct display_config get_display_config();

#ifdef __cplusplus
}
#endif

#endif
