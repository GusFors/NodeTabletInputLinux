#ifndef DISPLAYS_H
#define DISPLAYS_H

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

#ifdef __cplusplus
}
#endif

#endif