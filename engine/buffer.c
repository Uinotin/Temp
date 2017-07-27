#include "buffer.h"
#include "commands.h"


void UploadBufferFunc(char **data)
{
  Buffer *buffer = (Buffer *)(*data);
  glBindBuffer(buffer->handleData.type, buffer->handleData.handle);

  glBufferData(buffer->handleData.type, buffer->size, buffer->data, GL_STATIC_DRAW);
  (*data) += sizeof(Buffer);
}
void UploadBuffer(QueueData *queueData, Buffer *buffer)
{
  AppendParameter(queueData, buffer, sizeof(Buffer));
  AppendCommand(queueData, &UploadBufferFunc);
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
