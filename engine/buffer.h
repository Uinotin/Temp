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
  //char *swapData;
  char *data;
} Buffer;

typedef struct
{
  Buffer *buffers;
  TempUInt *handles;
  TempSizei nBuffers;
} BufferPtr;

void GenBuffers(BufferPtr *buffers, TempSizei nBuffers);

void SetBufferData(Buffer *buffer, TempSizei dataSize, char *data);

void UploadBuffersFunc(char **data);
void UploadBuffers(QueueData *queueData, BufferPtr *bufferPtr);

void BindBufferFunc(char **data);
void BindBuffer(QueueData *queueData, HandleData *handleData);

void UnbindBufferFunc(char **data);
void UnbindBuffer(QueueData *queueData, TempEnum bufferType);

//void SwapDataPointers(Buffer *buffer);

#endif
