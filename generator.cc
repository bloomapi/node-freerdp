#include <nan.h>

#include "generator.h"

using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using v8::Array;
using v8::Local;
using v8::String;
using v8::Value;

struct GeneratorBaton {
  uv_work_t request;     // libuv

  Callback *callback;    // javascript callback

  const GeneratorType *type;
  void *data;
};

// called by libuv worker in separate thread
static void DelayAsync(uv_work_t *req) { }

static void DelayAsyncAfter(uv_work_t *req, int status) {
  HandleScope scope;

  GeneratorBaton *baton = static_cast<GeneratorBaton *>(req->data);

  Local<Array> env_args = baton->type->arg_parser(baton->data);

  Local<Value> argv[] = {
    New<String>(baton->type->name).ToLocalChecked(),
    env_args
  };

  baton->callback->Call(2, argv);

  delete baton;
}

void generator_emit(GeneratorContext *generator, const GeneratorType *type, void * data) {
  // Create context for work queue
  GeneratorBaton *baton = new GeneratorBaton;
  baton->callback = generator->callback;
  baton->type = type;
  baton->data = data;

  baton->request.data = baton;

  uv_queue_work(uv_default_loop(), &baton->request, DelayAsync, DelayAsyncAfter);
}
