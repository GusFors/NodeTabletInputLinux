#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H
#include "../display.h"
#include "../tablet.h"

#ifdef __cplusplus
extern "C" {
#endif

// struct tablet_config get_tablet_config(const char *tablet_name);
struct tablet_config get_tablet_config(int vendor, int product);
struct tablet_config get_tablet_mmconfig(int vendor, int product);
void print_tablet_config(struct tablet_config cfg);
struct display_config get_display_config();

#ifdef __cplusplus
}
#endif

#endif