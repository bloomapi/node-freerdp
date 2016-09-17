#ifndef NODE_FREERDP_RDP_H
#define NODE_FREERDP_RDP_H

#include <nan.h>
#include <freerdp/freerdp.h>

#include "generator.h"

using Nan::Callback;

int node_freerdp_connect(int argc, char* argv[], Callback *callback);
void node_freerdp_send_key_event_scancode(int session_index, int code, int pressed);
void node_freerdp_send_pointer_event(int session_index, int flags, int x, int y);
void node_freerdp_set_clipboard(int session_index, void* data, int len);
void node_freerdp_close(int session);

#endif // NODE_FREERDP_RDP_H