#ifndef __NODE_FREERDP_CLIPRDR_H__
#define __NODE_FREERDP_CLIPRDR_H__

#include <freerdp/freerdp.h>

void node_cliprdr_init(freerdp* inst);
void node_cliprdr_uninit(freerdp* inst);
void node_process_cliprdr_set_clipboard_data(freerdp* inst, void* data, int len);
void node_process_cliprdr_event(freerdp* inst, wMessage* event);

#endif /* __NODE_FREERDP_CLIPRDR_H__ */