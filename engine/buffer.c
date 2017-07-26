#include "buffer.h"
#include "commands.h"


void UploadBufferFunc(char **data)
{
  struct Buffer *buffer = (struct Buffer *)data;
  glBindBuffer(buffer->handleData.type, buffer->handleData.handle);

  glBufferData(buffer->handleData.type, buffer->size, buffer->data, GL_STATIC_DRAW);
  *data += sizeof(struct Buffer);
}
void UploadBuffer(struct QueueData *queueData, struct Buffer *buffer)
{
  AppendParameter(queueData, buffer, sizeof(struct Buffer));
  AppendCommand(queueData, &UploadBufferFunc);
}

void BindBufferFunc(char **data)
{
  struct HandleData *handleData = (struct HandleData *)data;
  glBindBuffer(handleData->type, handleData->handle);
  *data += sizeof(struct HandleData);
}
void BindBuffer(struct QueueData *queueData, struct HandleData *handleData)
{
  AppendParameter(queueData, &(handleData), sizeof(struct HandleData));
  AppendCommand(queueData, &BindBufferFunc);
}

void UnbindBufferFunc(char **data)
{
  glBindBuffer(*((TempEnum*)data), 0);
  *data += sizeof(TempEnum);
}

void UnbindBuffer(struct QueueData *queueData, TempEnum bufferType)
{
  AppendParameter(queueData, &bufferType, sizeof(TempEnum));
  AppendCommand(queueData, &UnbindBufferFunc);
}
