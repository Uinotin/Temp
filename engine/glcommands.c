#include "commands.h"
#include "glrend.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

size_t ClearFunc(struct Rend *rend, char *data)
{
  GLenum bitEnum = *((GLenum *)data);
  glClear(bitEnum);
  return sizeof(GLenum);
}

size_t GenBufferFunc(struct Rend *rend, char *data)
{
  TempUInt glHandle;
  glGenBuffers(1, &glHandle);
  SetGLHandle((struct GLRend *)(rend->rend), *((TempUInt*)data), glHandle);
  return sizeof(TempUInt);
}

void GenBuffer(struct CommandQueue *commandQueue, TempUInt handle)
{
  struct QueueData *queueData = &(commandQueue->queueData);
  assert(queueData->queueLen + sizeof (TempUInt) < commandQueue->queueSize && queueData->nCommands < queueData->maxCommands);
  queueData->queue[commandQueue->queueData.queueLen] = handle;
  queueData->queue += sizeof(TempUInt);
  queueData->commands[(queueData->nCommands++)] = &GenBuffer; 
}

size_t BindBufferFunc(struct Rend *rend, char *data)
{
  GLenum bufferEnum = *((GLenum *)data);
  data += sizeof(GLenum);
  glBindBuffer(bufferEnum, GetGLHandle((struct GLRend *)(rend->rend), *((TempUInt *)data)));
  return sizeof(GLenum) + sizeof(TempUInt);
}
