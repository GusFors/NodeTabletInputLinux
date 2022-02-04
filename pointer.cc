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

NAN_METHOD(getNumberOfMonitors)
{
  if (display == NULL)
  {
      display = XOpenDisplay(NULL);
  }

  int numMonitors = 0;
	XRRGetMonitors(display, DefaultRootWindow(display), 1, &numMonitors);
  info.GetReturnValue().Set(Nan::New(numMonitors));
}

NAN_METHOD(getDisplaysTotalWidth)
{

  if (display == NULL)
  {
      display = XOpenDisplay(NULL);
  }

  int x = 0;
  int displayWidth = XDisplayWidth(display, x);

  info.GetReturnValue().Set(Nan::New(displayWidth)); 
}

NAN_METHOD(getPrimaryMonitorXoffset)
{
  if (display == NULL)
  {
      display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, DefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);
 
  info.GetReturnValue().Set(Nan::New(monInfo->x)); 
}

NAN_METHOD(getPrimaryMonitorYoffset)
{
  if (display == NULL)
  {
      display = XOpenDisplay(NULL);
  }

  XRRScreenResources *monRes = XRRGetScreenResources(display, DefaultRootWindow(display));
  XRRCrtcInfo *monInfo = XRRGetCrtcInfo(display, monRes, monRes->crtcs[0]);
 
  info.GetReturnValue().Set(Nan::New(monInfo->y)); 
}


NAN_MODULE_INIT(init)
{
    Nan::SetMethod(target, "setPointer", setPointer);
    Nan::SetMethod(target, "getDisplaysTotalWidth", getDisplaysTotalWidth);
    Nan::SetMethod(target, "getNumberOfMonitors", getNumberOfMonitors);
    Nan::SetMethod(target, "getPrimaryMonitorXoffset", getPrimaryMonitorXoffset);
    Nan::SetMethod(target, "getPrimaryMonitorYoffset", getPrimaryMonitorYoffset);
}

NODE_MODULE(pointer, init);