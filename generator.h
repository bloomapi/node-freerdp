#ifndef NODE_ASYNC_LEARN_GENERATOR_H
#define NODE_ASYNC_LEARN_GENERATOR_H

#include <nan.h>

using Nan::Callback;
using v8::Local;
using v8::Value;
using v8::Array;

struct GeneratorContext {
  Callback *callback;  // javascript callback
};

struct GeneratorType {
  const char *name;
  Local<Array> (*arg_parser)(void *);
};

void generator_emit(GeneratorContext *generator, const GeneratorType *type, void *data);

#endif // NODE_ASYNC_LEARN_GENERATOR_H
