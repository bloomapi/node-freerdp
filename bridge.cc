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

  int sessionIndex = rdpConnect(jsArray->Length() + 1, cstrings, callback);
  info.GetReturnValue().Set(sessionIndex);
}

NAN_METHOD(SendKeyEventScancode) {
  Nan::HandleScope scope;

  int sessionIndex = info[0]->Uint32Value();
  int scanCode = info[1]->Uint32Value();
  int pressed = info[2]->Uint32Value();
  send_key_event_scancode(sessionIndex, scanCode, pressed);
}

NAN_METHOD(SendPointerEvent) {
  Nan::HandleScope scope;

  int sessionIndex = info[0]->Uint32Value();
  int x = info[1]->Uint32Value();
  int y = info[2]->Uint32Value();
  int button = info[3]->Uint32Value();
  int pressed = info[2]->Uint32Value();
  
  send_pointer_event(sessionIndex, x, y, button, pressed);
}

NAN_METHOD(Close) {
  Nan::HandleScope scope;
}