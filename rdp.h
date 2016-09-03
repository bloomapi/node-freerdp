#include <nan.h>

using Nan::Callback;

int rdpConnect(int argc, char* argv[], Callback *callback);

void send_key_event_scancode(int session_index, int code, int pressed);
void send_pointer_event(int session_index, int x, int y, int button, int pressed);