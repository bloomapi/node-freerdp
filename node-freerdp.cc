#include <nan.h>
#include "bridge.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("connect").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(Connect)).ToLocalChecked());

  Set(target, New<String>("sendKeyEventScancode").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(SendKeyEventScancode)).ToLocalChecked());

  Set(target, New<String>("sendPointerEvent").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(SendPointerEvent)).ToLocalChecked());

  Set(target, New<String>("setClipboard").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(SetClipboard)).ToLocalChecked());

  Set(target, New<String>("close").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(Close)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)