#ifndef BUFFER_H
#define BUFFER_H
#include "typedefs.h"
#include "commandqueue.h"

struct HandleData
{
  TempUInt handle;
  TempEnum type;
};

struct Buffer
{
  struct HandleData handleData;
  size_t size;
  char *data;
};

void UploadBufferFunc(char **data);
void UploadBuffer(struct QueueData *queueData, struct Buffer *buffer);

void BindBufferFunc(char **data);
void BindBuffer(struct QueueData *queueData, struct HandleData *handleData);

void UnbindBufferFunc(char **data);
void UnbindBuffer(struct QueueData *queueData, TempEnum bufferType);

#endif
