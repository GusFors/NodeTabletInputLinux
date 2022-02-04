#include <nan.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

using namespace v8;

Display *display = NULL;

// TODO fix an actual formatter for c++
NAN_METHOD(setPointer)
{

    int32_t x = Nan::To<int32_t>(info[0]).FromJust();
    int32_t y = Nan::To<int32_t>(info[1]).FromJust();
  

    if (display == NULL)
    {
        display = XOpenDisplay(":0.0");
    }

    XWarpPointer(display, None, DefaultRootWindow(display), 0, 0, 0, 0, x, y);
    XSync(display, false);

    // XFlush(display);
    // XCloseDisplay(display);
    info.GetReturnValue().Set(Nan::New(1));
}

NAN_MODULE_INIT(init)
{
    Nan::SetMethod(target, "setPointer", setPointer);
}

NODE_MODULE(pointer, init);