#include <stdlib.h>
#include "vertexarray.h"
#include "commands.h"

void GenVAOsFunc(char **data)
{
  TempSizei size = *((TempSizei *)*data);
  *data += sizeof(TempSizei);
  VertexArrayPtr *VAOPtr = (VertexArrayPtr *)*data;
  *data += sizeof(VertexArrayPtr);
  glGenVertexArrays(size, VAOPtr->vaoHandle);
}

void GenVAOs(QueueData *queueData, TempSizei nVertexArrays, VertexArrayPtr *vertexArrayPtr)
{
  vertexArrayPtr->vaoHandle = (TempUInt *)malloc(nVertexArrays * sizeof(TempUInt));
  AppendParameter(queueData, &nVertexArrays, sizeof(TempSizei));
  AppendParameter(queueData, vertexArrayPtr, sizeof(VertexArrayPtr));
  AppendCommand(queueData, &GenVAOsFunc);
}

void BindVAOPtrFunc(char **data)
{
  VertexArrayPtr *VAOPtr = (VertexArrayPtr *)*data;
  *data += sizeof(VertexArrayPtr);
  glBindVertexArray(*(VAOPtr->vaoHandle));
}
void BindVAOPtr(QueueData *queueData, VertexArrayPtr *VAOPtr)
{
  AppendParameter(queueData, VAOPtr, sizeof(VertexArrayPtr));
  AppendCommand(queueData, &BindVAOPtrFunc);
}

void BindVAOFunc(char **data)
{
  TempUInt handle = *((TempUInt *)*data);
  *data += sizeof(TempUInt);
  glBindVertexArray(handle);
}
void BindVAO(QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &BindVAOFunc);
}
/*
Attrib AllocAttribPointer(Buffer *buffer, Attrib *attrib)
{
  TempVoid *data;
  pthread_mutex_lock(attrib->mutex);
  data = (TempVoid *)((buffer->data) + (char *)(attrib->offset));
  return {.size = attrib->size, .type = attrib->type, .bufferType = attrib->bufferType, .offset = attrib->offset, .data = data, .mutex = attrib->mutex};
}

void FreeAttribPointer(Attrib *attrib)
{
  pthread_mutex_unlock(attrib->mutex);
}

void BufferSubDataFunc(char **data)
{
  Attrib *attrib = (Attrib *)*data;
  *data += sizeof(Attrib);
  glBufferSubData(attrib->bufferType, attrib->offset, (attrib->size) * sizeof(attrib->type), attrib->data);
}
void BufferSubData(QueueData *queueData, Attrib *attrib)
{
  AppendParameter(queueData, attrib, sizeof(Attrib));
  AppendCommand(queueData, &BufferSubDataFunc);
}
*/
