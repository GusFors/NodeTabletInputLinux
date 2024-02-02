#ifndef DISPLAY_H
#define DISPLAY_H

struct display_config {
  int total_width;
  int total_height;
  int xoffset;
  int yoffset;
  int primary_height;
  int primary_width;
};

#ifdef __cplusplus
extern "C" {
#endif

int get_displays_total_width();
int get_displays_total_height();
int get_primary_monitor_xoffset();
int get_primary_monitor_yoffset();
int get_primary_monitor_width();
int get_primary_monitor_height();
int get_number_of_monitors();
int close_display();

#ifdef __cplusplus
}
#endif

#endif