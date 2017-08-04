#include <stdlib.h>
#include "buffer.h"
#include "commands.h"

void GenBuffers(BufferPtr *buffers, TempSizei nBuffers)
{
  char *tempPtr = (char *)malloc(sizeof(Buffer)*nBuffers + sizeof(TempUInt)*nBuffers);
  buffers->buffers = (Buffer *)tempPtr;
  buffers->handles = (TempUInt *)(tempPtr + sizeof(Buffer)*nBuffers);
  buffers->nBuffers = nBuffers;
}


void SetBufferData(Buffer *buffer, TempSizei dataSize, char *data)
{
  buffer->size = dataSize;
  buffer->data = data;
}

void UploadBuffersFunc(char **data)
{
  BufferPtr *bufferPtr;
  Buffer *buffers;
  TempSizei i;
  bufferPtr = (BufferPtr *)*data;
  *data += sizeof(BufferPtr);
  glGenBuffers(bufferPtr->nBuffers, bufferPtr->handles);
  buffers = bufferPtr->buffers;

  for (i = 0; i < bufferPtr->nBuffers; ++i)
  {
    buffers[i].handleData.handle = bufferPtr->handles[i];
    if(buffers[i].size)
    {
      glBindBuffer(buffers[i].handleData.type, buffers[i].handleData.handle);
      glBufferData(buffers[i].handleData.type, buffers[i].size, buffers[i].data, GL_STATIC_DRAW);
    }
  }
}
void UploadBuffers(QueueData *queueData, BufferPtr *bufferPtr)
{
  AppendParameter(queueData, bufferPtr, sizeof(BufferPtr));
  AppendCommand(queueData, &UploadBuffersFunc);
}

void BindBufferFunc(char **data)
{
  HandleData *handleData = (HandleData *)(*data);
  glBindBuffer(handleData->type, handleData->handle);
  *data += sizeof(HandleData);
}
void BindBuffer(QueueData *queueData, HandleData *handleData)
{
  AppendParameter(queueData, &(handleData), sizeof(HandleData));
  AppendCommand(queueData, &BindBufferFunc);
}

void UnbindBufferFunc(char **data)
{
  glBindBuffer(*((TempEnum*)(*data)), 0);
  *data += sizeof(TempEnum);
}

void UnbindBuffer(QueueData *queueData, TempEnum bufferType)
{
  AppendParameter(queueData, &bufferType, sizeof(TempEnum));
  AppendCommand(queueData, &UnbindBufferFunc);
}
