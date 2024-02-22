#ifndef CONFIG_HANDLER_H
#define CONFIG_HANDLER_H
#include "../display.h"
#include "../tablet.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tablet_config get_tablet_config(const char *tablet_name);
struct display_config get_display_config();

#ifdef __cplusplus
}
#endif

#endif