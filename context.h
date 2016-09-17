#ifndef NODE_FREERDP_CONTEXT_H
#define NODE_FREERDP_CONTEXT_H

#include <freerdp/freerdp.h>

#include "generator.h"

struct node_info
{
  void* data;
};
typedef struct node_info nodeInfo;

struct node_context
{
  rdpContext _p;

  nodeInfo* nodei;
  GeneratorContext *generatorContext;

  //ANDROID_EVENT_QUEUE* event_queue;
  //pthread_t thread;
  //BOOL is_connected;

  void* clipboard_context;
};
typedef struct node_context nodeContext;

#endif // NODE_FREERDP_CONTEXT_H
