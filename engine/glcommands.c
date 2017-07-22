#include "commands.h"
#include "glrend.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void AppendParameter(struct QueueData *queueData, void *parameter, size_t len)
{
  assert(queueData->queueLen + len < queueData->queueSize);
  //FIXME: This might cause performance issues if used with non-array parametres?
  memcpy(queueData->queue + queueData->queueLen, parameter, len);
  queueData->queueLen += len;
}

static void AppendCommand(struct QueueData *queueData, Command command)
{
  assert(queueData->nCommands < queueData->maxCommands);
  queueData->commands[(queueData->nCommands++)] = command; 
}


size_t ClearFunc(struct Rend *rend, char *data)
{
  GLenum bitEnum = *((GLenum *)data);
  glClear(bitEnum);
  return sizeof(GLenum);
}
		       
void Clear(struct QueueData *queueData, TempEnum bitEnum)
{
  AppendParameter(queueData, &bitEnum, sizeof(TempEnum));
  AppendCommand(queueData, &ClearFunc);
}


size_t GenBufferFunc(struct Rend *rend, char *data)
{
  TempUInt glHandle;
  glGenBuffers(1, &glHandle);
  SetGLHandle((struct GLRend *)(rend->rend), *((TempUInt*)data), glHandle);
  return sizeof(TempUInt);
}

void GenBuffer(struct QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &GenBufferFunc);
}


size_t BindBufferFunc(struct Rend *rend, char *data)
{
  GLenum bufferEnum = *((GLenum *)data);
  data += sizeof(GLenum);
  glBindBuffer(bufferEnum, GetGLHandle((struct GLRend *)(rend->rend), *((TempUInt *)data)));
  return sizeof(GLenum) + sizeof(TempUInt);
}

void BindBuffer(struct QueueData *queueData, TempEnum bufferType, TempUInt handle)
{
  AppendParameter(queueData, &bufferType, sizeof(TempEnum));
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &BindBufferFunc);
}
