#include <nan.h>

#include "bridge.h"
#include "rdp.h"

using v8::Function;
using v8::Local;
using v8::Number;
using v8::Value;
using v8::String;
using v8::Handle;
using v8::Array;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::MaybeLocal;
using Nan::Null;
using Nan::To;

NAN_METHOD(Connect) {
  Nan::HandleScope scope;
  Handle<Value> val;

  Handle<Array> jsArray = Handle<Array>::Cast(info[0]);
  char** cstrings = new char*[jsArray->Length() + 1];

  std::string tmp = "node-freerdp";
  cstrings[0] = new char[tmp.size() + 1];
  std::strcpy(cstrings[0], tmp.c_str());

  for (unsigned int i = 0; i < jsArray->Length(); i++) {
    val = jsArray->Get(i);
    std::string current = std::string(*String::Utf8Value(val));
    cstrings[i + 1] = new char[current.size() + 1];
    std::strcpy(cstrings[i + 1], current.c_str());
  }

  Callback *callback = new Callback(info[1].As<Function>());

  int session_index = node_freerdp_connect(jsArray->Length() + 1, cstrings, callback);
  info.GetReturnValue().Set(session_index);
}

NAN_METHOD(SendKeyEventScancode) {
  Nan::HandleScope scope;

  int session_index = info[0]->Uint32Value();
  int scanCode = info[1]->Uint32Value();
  int pressed = info[2]->Uint32Value();

  node_freerdp_send_key_event_scancode(session_index, scanCode, pressed);
}

NAN_METHOD(SendPointerEvent) {
  Nan::HandleScope scope;

  int session_index = info[0]->Uint32Value();
  int flags = info[1]->Uint32Value();
  int x = info[2]->Uint32Value();
  int y = info[3]->Uint32Value();

  node_freerdp_send_pointer_event(session_index, flags, x, y);
}


NAN_METHOD(SetClipboard) {
  Nan::HandleScope scope;

  int session_index = info[0]->Uint32Value();
  std::string str = std::string(*String::Utf8Value(info[1]));

  node_freerdp_set_clipboard(session_index, (void*)str.c_str(), str.size());
}

NAN_METHOD(Close) {
  Nan::HandleScope scope;

  int session_index = info[0]->Uint32Value();

  node_freerdp_close(session_index);
}
