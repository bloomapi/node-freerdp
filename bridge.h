#ifndef NODE_FREERDP_BRIDGE_H
#define NODE_FREERDP_BRIDGE_H

#include <nan.h>

NAN_METHOD(Connect);
NAN_METHOD(SendKeyEventScancode);
NAN_METHOD(SendPointerEvent);
NAN_METHOD(SetClipboard);
NAN_METHOD(Close);

#endif  // NODE_FREERDP_BRIDGE_H