#ifndef BUFFER_H
#define BUFFER_H
#include "typedefs.h"
#include "commandqueue.h"

typedef struct
{
  TempUInt handle;
  TempEnum type;
} HandleData;

typedef struct
{
  HandleData handleData;
  size_t size;
  char *data;
} Buffer;

void UploadBufferFunc(char **data);
void UploadBuffer(QueueData *queueData, Buffer *buffer);

void BindBufferFunc(char **data);
void BindBuffer(QueueData *queueData, HandleData *handleData);

void UnbindBufferFunc(char **data);
void UnbindBuffer(QueueData *queueData, TempEnum bufferType);

#endif
