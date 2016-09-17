#include <stdlib.h>

#include <winpr/crt.h>
#include <winpr/stream.h>

#include <freerdp/utils/event.h>
#include <freerdp/client/channels.h>
#include <freerdp/client/cliprdr.h>

#include "cliprdr.h"

#include "context.h"
#include "rdp.h"

typedef struct clipboard_context clipboardContext;
struct clipboard_context
{
  freerdp* instance;
  rdpChannels* channels;
  
  /* server->client data */
  UINT32* formats;
  int num_formats;  
  BYTE* data;
  UINT32 data_format;
  int data_length;

  /* client->server data */
  UINT32* node_formats;
  int   node_num_formats;
  BYTE* node_data;
  int node_data_length;
};

static BYTE* lf2crlf(BYTE* data, int* size)
{
  BYTE c;
  BYTE* outbuf;
  BYTE* out;
  BYTE* in_end;
  BYTE* in;
  int out_size;

  out_size = (*size) * 2 + 1;
  outbuf = (BYTE*) malloc(out_size);
  ZeroMemory(outbuf, out_size);

  out = outbuf;
  in = data;
  in_end = data + (*size);

  while (in < in_end)
  {
    c = *in++;
    if (c == '\n')
    {
      *out++ = '\r';
      *out++ = '\n';
    }
    else
    {
      *out++ = c;
    }
  }

  *out++ = 0;
  *size = out - outbuf;

  return outbuf;
}

static void crlf2lf(BYTE* data, int* size)
{
  BYTE c;
  BYTE* out;
  BYTE* in;
  BYTE* in_end;

  out = data;
  in = data;
  in_end = data + (*size);

  while (in < in_end)
  {
    c = *in++;

    if (c != '\r')
      *out++ = c;
  }

  *size = out - data;
}

static void be2le(BYTE* data, int size)
{
  BYTE c;

  while (size >= 2)
  {
    c = data[0];
    data[0] = data[1];
    data[1] = c;

    data += 2;
    size -= 2;
  }
}

void node_cliprdr_init(freerdp* inst)
{
  nodeContext* ctx = (nodeContext*)inst->context;
  clipboardContext* cb;

  cb = (clipboardContext*)malloc(sizeof(clipboardContext));
  ZeroMemory(cb, sizeof(clipboardContext));
  cb->instance = inst;
  cb->channels = inst->context->channels;
  
  cb->node_formats = (UINT32*)malloc(sizeof(UINT32) * 3);
  cb->node_formats[0] = CB_FORMAT_TEXT;
  cb->node_formats[1] = CB_FORMAT_UNICODETEXT;
  cb->node_formats[2] = CB_FORMAT_HTML;
  cb->node_num_formats = 3;

#if 0 
  cb->node_data = strdup("node_CLIPBOARD_TEST");
  cb->node_data_length = strlen(cb->node_data);
#endif
  
  ctx->clipboard_context = cb;
}

void node_cliprdr_uninit(freerdp* inst)
{
  nodeContext* ctx = (nodeContext*)inst->context;
  clipboardContext* cb = (clipboardContext*)ctx->clipboard_context;
  
  if (cb)
  {
    if (cb->formats)
      free(cb->formats);
    if (cb->data)
      free(cb->data);
    if (cb->node_formats)
      free(cb->node_formats);
    if (cb->node_data)
      free(cb->node_data);
    free(cb);
    ctx->clipboard_context = NULL;
  }
}

static void node_cliprdr_send_null_format_list(clipboardContext* cb)
{
  RDP_CB_FORMAT_LIST_EVENT* event;

  event = (RDP_CB_FORMAT_LIST_EVENT*) freerdp_event_new(CliprdrChannel_Class,
    CliprdrChannel_FormatList, NULL, NULL);

  event->num_formats = 0;

  freerdp_channels_send_event(cb->channels, (wMessage*) event);
}

static void node_cliprdr_send_supported_format_list(clipboardContext* cb)
{
  int i;
  RDP_CB_FORMAT_LIST_EVENT* event;

  event = (RDP_CB_FORMAT_LIST_EVENT*) freerdp_event_new(CliprdrChannel_Class,
    CliprdrChannel_FormatList, NULL, NULL);

  event->formats = (UINT32*) malloc(sizeof(UINT32) * cb->node_num_formats);
  event->num_formats = cb->node_num_formats;

  for (i = 0; i < cb->node_num_formats; i++)
  {
    event->formats[i] = cb->node_formats[i];
  }

  freerdp_channels_send_event(cb->channels, (wMessage*) event);
}

static void node_cliprdr_send_format_list(clipboardContext* cb)
{
  if (cb->node_data)
  {
    node_cliprdr_send_supported_format_list(cb);
  }
  else
  {
    node_cliprdr_send_null_format_list(cb);
  }
}

static void node_cliprdr_send_data_request(clipboardContext* cb, UINT32 format)
{
  RDP_CB_DATA_REQUEST_EVENT* event;

  event = (RDP_CB_DATA_REQUEST_EVENT*) freerdp_event_new(CliprdrChannel_Class,
    CliprdrChannel_DataRequest, NULL, NULL);

  event->format = format;

  freerdp_channels_send_event(cb->channels, (wMessage*) event);
}

static void node_cliprdr_send_data_response(clipboardContext* cb, BYTE* data, int size)
{
  RDP_CB_DATA_RESPONSE_EVENT* event;

  event = (RDP_CB_DATA_RESPONSE_EVENT*) freerdp_event_new(CliprdrChannel_Class,
    CliprdrChannel_DataResponse, NULL, NULL);

  event->data = data;
  event->size = size;

  freerdp_channels_send_event(cb->channels, (wMessage*) event);
}

static void node_cliprdr_send_null_data_response(clipboardContext* cb)
{
  node_cliprdr_send_data_response(cb, NULL, 0);
}

static void node_cliprdr_process_cb_monitor_ready_event(clipboardContext* cb)
{
  node_cliprdr_send_format_list(cb);
}

static BYTE* node_cliprdr_process_requested_unicodetext(BYTE* data, int* size)
{
  char* inbuf;
  WCHAR* outbuf = NULL;
  int out_size;

  inbuf = (char*) lf2crlf(data, size);
  out_size = ConvertToUnicode(CP_UTF8, 0, inbuf, -1, &outbuf, 0);
  free(inbuf);

  *size = (int) ((out_size + 1) * 2);

  return (BYTE*) outbuf;
}

static BYTE* node_cliprdr_process_requested_text(BYTE* data, int* size)
{
  BYTE* outbuf;

  outbuf = lf2crlf(data, size);

  return outbuf;
}

static BYTE* node_cliprdr_process_requested_html(BYTE* data, int* size)
{
  char* inbuf;
  BYTE* in;
  BYTE* outbuf;
  char num[11];

  inbuf = NULL;

  if (*size > 2)
  {
    if ((BYTE) data[0] == 0xFE && (BYTE) data[1] == 0xFF)
    {
      be2le(data, *size);
    }

    if ((BYTE) data[0] == 0xFF && (BYTE) data[1] == 0xFE)
    {
      ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) (data + 2), (*size - 2) / 2, &inbuf, 0, NULL, NULL);
    }
  }

  if (inbuf == NULL)
  {
    inbuf = (char*)malloc(*size + 1);
    ZeroMemory(inbuf, *size + 1);

    memcpy(inbuf, data, *size);
  }

  outbuf = (BYTE*) malloc(*size + 200);
  ZeroMemory(outbuf, *size + 200);

  strcpy((char*) outbuf,
    "Version:0.9\r\n"
    "StartHTML:0000000000\r\n"
    "EndHTML:0000000000\r\n"
    "StartFragment:0000000000\r\n"
    "EndFragment:0000000000\r\n");

  in = (BYTE*) strstr((char*) inbuf, "<body");

  if (in == NULL)
  {
    in = (BYTE*) strstr((char*) inbuf, "<BODY");
  }
  /* StartHTML */
  snprintf(num, sizeof(num), "%010lu", (unsigned long) strlen((char*) outbuf));
  memcpy(outbuf + 23, num, 10);
  if (in == NULL)
  {
    strcat((char*) outbuf, "<HTML><BODY>");
  }
  strcat((char*) outbuf, "<!--StartFragment-->");
  /* StartFragment */
  snprintf(num, sizeof(num), "%010lu", (unsigned long) strlen((char*) outbuf));
  memcpy(outbuf + 69, num, 10);
  strcat((char*) outbuf, (char*) inbuf);
  /* EndFragment */
  snprintf(num, sizeof(num), "%010lu", (unsigned long) strlen((char*) outbuf));
  memcpy(outbuf + 93, num, 10);
  strcat((char*) outbuf, "<!--EndFragment-->");
  if (in == NULL)
  {
    strcat((char*) outbuf, "</BODY></HTML>");
  }
  /* EndHTML */
  snprintf(num, sizeof(num), "%010lu", (unsigned long) strlen((char*) outbuf));
  memcpy(outbuf + 43, num, 10);

  *size = strlen((char*) outbuf) + 1;
  free(inbuf);

  return outbuf;
}

static void node_cliprdr_process_cb_data_request_event(clipboardContext* cb, RDP_CB_DATA_REQUEST_EVENT* event)
{
  int i;

  //printf("DEBUG: format %d\n", event->format);

  for(i = 0; i < cb->node_num_formats; i++)
  {
    if (event->format == cb->node_formats[i])
      break;
  }

  if (i >= cb->node_num_formats)
  {
    printf("DEBUG: unsupported format requested");
    node_cliprdr_send_null_data_response(cb);
  } 
  else if (!cb->node_data)
  {
    printf("DEBUG: no node clipdata");
    node_cliprdr_send_null_data_response(cb);
  }
  else
  {
    BYTE* outbuf;
    int size = cb->node_data_length;

    switch (event->format)
    {
      case CB_FORMAT_RAW:
      case CB_FORMAT_PNG:
      case CB_FORMAT_JPEG:
      case CB_FORMAT_GIF:
      case CB_FORMAT_DIB:
      default:
        printf("DEBUG: unsupported format %x\n", event->format);
        outbuf = NULL;
        break;

      case CB_FORMAT_UNICODETEXT:
        outbuf = node_cliprdr_process_requested_unicodetext(cb->node_data, &size);
        break;

      case CB_FORMAT_TEXT:
        outbuf = node_cliprdr_process_requested_text(cb->node_data, &size);
        break;

      case CB_FORMAT_HTML:
        outbuf = node_cliprdr_process_requested_html(cb->node_data, &size);
        break;
    }

    if (outbuf) {
      node_cliprdr_send_data_response(cb, outbuf, size);
    }
    else {
      node_cliprdr_send_null_data_response(cb);
    }
  }
  
  /* Resend the format list, otherwise the server won't request again for the next paste */
  node_cliprdr_send_format_list(cb);
}

static BOOL node_cliprdr_has_format(UINT32* formats, int num_formats, UINT32 format)
{
  int i;
  for(i = 0; i < num_formats; i++)
  {
    if (formats[i] == format)
      return TRUE;
  }
  return FALSE;
}

static void node_cliprdr_process_cb_format_list_event(clipboardContext* cb, RDP_CB_FORMAT_LIST_EVENT* event)
{
  if (cb->data)
  {
    free(cb->data);
    cb->data = NULL;
    cb->data_length = 0;
  }

  if (cb->formats)
    free(cb->formats);

  cb->data_format = CB_FORMAT_RAW;
  cb->formats = event->formats;
  cb->num_formats = event->num_formats;
  event->formats = NULL;
  event->num_formats = 0;

  if (node_cliprdr_has_format(cb->formats, cb->num_formats, CB_FORMAT_TEXT))
  {
    cb->data_format = CB_FORMAT_TEXT;
    node_cliprdr_send_data_request(cb, CB_FORMAT_TEXT);    
  }
  else if (node_cliprdr_has_format(cb->formats, cb->num_formats, CB_FORMAT_UNICODETEXT))
  {
    cb->data_format = CB_FORMAT_UNICODETEXT;
    node_cliprdr_send_data_request(cb, CB_FORMAT_UNICODETEXT);
  }
  else if (node_cliprdr_has_format(cb->formats, cb->num_formats, CB_FORMAT_HTML))
  {
    cb->data_format = CB_FORMAT_HTML;
    node_cliprdr_send_data_request(cb, CB_FORMAT_HTML);
  }
}

static void node_cliprdr_process_text(clipboardContext* cb, BYTE* data, int size)
{
  if (size > 0 && data)
  {
    cb->data = (BYTE*) malloc(size + 1);
    memcpy(cb->data, data, size);
    cb->data[size] = 0;
    cb->data_length = size;
  }
}

static void node_cliprdr_process_unicodetext(clipboardContext* cb, BYTE* data, int size)
{
  cb->data_length = ConvertFromUnicode(CP_UTF8, 0, (WCHAR*) data, size / 2, (CHAR**) &(cb->data), 0, NULL, NULL);
  crlf2lf(cb->data, &cb->data_length);
}

static void node_cliprdr_process_html(clipboardContext* cb, BYTE* data, int size)
{
  char* start_str;
  char* end_str;
  int start;
  int end;

  start_str = strstr((char*) data, "StartHTML:");
  end_str = strstr((char*) data, "EndHTML:");
  if (start_str == NULL || end_str == NULL)
  {
    printf("DEBUG: invalid HTML clipboard format");
    return;
  }
  start = atoi(start_str + 10);
  end = atoi(end_str + 8);
  if (start > size || end > size || start >= end)
  {
    printf("DEBUG: invalid HTML offset");
    return;
  }

  cb->data = (BYTE*) malloc(end - start + 1);
  memcpy(cb->data, data + start, end - start);
  cb->data[end - start] = 0;
  cb->data_length = end - start;
}

static void node_cliprdr_process_cb_data_response_event(clipboardContext* cb, RDP_CB_DATA_RESPONSE_EVENT* event)
{
  //printf("DEBUG: size=%d", event->size);

  if (event->size > 0)
  {
    if (cb->data)
    {
      free(cb->data);
      cb->data = NULL;
      cb->data_length = 0;
    }
    switch (cb->data_format)
    {
      case CB_FORMAT_RAW:
      case CB_FORMAT_PNG:
      case CB_FORMAT_JPEG:
      case CB_FORMAT_GIF:
      case CB_FORMAT_DIB:
      default:
        printf("DEBUG: unsupported format\n");
        break;

      case CB_FORMAT_TEXT:
        node_cliprdr_process_text(cb, event->data, event->size - 1);
        break;

      case CB_FORMAT_UNICODETEXT:
        node_cliprdr_process_unicodetext(cb, event->data, event->size - 2);
        break;

      case CB_FORMAT_HTML:
        node_cliprdr_process_html(cb, event->data, event->size);
        break;
    }
    
    //printf("DEBUG: computer_clipboard_data %s ", (char*)cb->data);    
    if (cb->data)
    {   //TODO: CALLBACK
      /*JNIEnv* env;
      jboolean attached = jni_attach_thread(&env);
      jstring jdata = jniNewStringUTF(env, cb->data, cb->data_length);

      freerdp_callback("OnRemoteClipboardChanged", "(ILjava/lang/String;)V", cb->instance, jdata);

      (*env)->DeleteLocalRef(env, jdata);
      if(attached == JNI_TRUE)
      {
        jni_detach_thread();
      }*/
    }
  }
}

void node_process_cliprdr_event(freerdp* inst, wMessage* event)
{
  nodeContext* ctx = (nodeContext*)inst->context;
  clipboardContext* cb = (clipboardContext*) ctx->clipboard_context;
  
  if (!cb)
  {
    return;
  }
  
  switch (GetMessageType(event->id))
  {
    case CliprdrChannel_MonitorReady:
      node_cliprdr_process_cb_monitor_ready_event(cb);
      break;

    case CliprdrChannel_FormatList:
      node_cliprdr_process_cb_format_list_event(cb, (RDP_CB_FORMAT_LIST_EVENT*) event);
      break;

    case CliprdrChannel_DataRequest:
      node_cliprdr_process_cb_data_request_event(cb, (RDP_CB_DATA_REQUEST_EVENT*) event);
      break;

    case CliprdrChannel_DataResponse:
      node_cliprdr_process_cb_data_response_event(cb, (RDP_CB_DATA_RESPONSE_EVENT*) event);
      break;

    default:
      printf("DEBUG: unknown event type %d", GetMessageType(event->id));
      break;
  }
}

void node_process_cliprdr_set_clipboard_data(freerdp* inst, void* data, int len)
{
  nodeContext* ctx = (nodeContext*)inst->context;
  clipboardContext* cb = (clipboardContext*) ctx->clipboard_context;

  //printf("DEBUG: node_clipboard_data %s\n", (char*)data);
    
  if (cb && (data == NULL || cb->node_data == NULL || len != cb->node_data_length || memcmp(data, cb->node_data, len)))
  {
    if (cb->node_data)
    {
      free(cb->node_data);
      cb->node_data = NULL;
      cb->node_data_length = 0;
    }
    if (data)
    {
      cb->node_data = (BYTE*)malloc(len + 1);
      memcpy(cb->node_data, data, len);
      cb->node_data[len] = 0;
      cb->node_data_length = len;
    }

    node_cliprdr_send_format_list(cb);
  }
}
