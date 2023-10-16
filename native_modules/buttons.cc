#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <nan.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XTest.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>

Display *display = NULL;
Window root = 0;
int32_t fd;
struct uinput_user_dev uiPointer;
struct uinput_abs_setup uiPressure;
struct input_absinfo uInfo;