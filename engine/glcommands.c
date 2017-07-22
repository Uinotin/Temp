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
  TempUInt handle = *((TempUInt *)data);
  glBindBuffer(bufferEnum, handle ? GetGLHandle((struct GLRend *)(rend->rend), *((TempUInt *)data)) : 0);
  return sizeof(GLenum) + sizeof(TempUInt);
}

void BindBuffer(struct QueueData *queueData, TempEnum bufferType, TempUInt handle)
{
  AppendParameter(queueData, &bufferType, sizeof(TempEnum));
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &BindBufferFunc);
}

size_t GenVAOFunc(struct Rend *rend, char *data)
{
  TempUInt glHandle;
  glGenVertexArrays(1, &glHandle);
  SetGLHandle((struct GLRend *)(rend->rend), *((TempUInt*)data), glHandle);
  return sizeof(TempUInt);
}

void GenVAO(struct QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &GenVAOFunc);
}

size_t BindVAOFunc(struct Rend *rend, char *data)
{
  glBindVertexArray(handle ? GetGLHandle((struct GLRend *)(rend->rend), *((TempUInt *)data)) : 0);
  return sizeof(TempUInt);
}
void BindVAO(struct QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &BindVAOFunc);
}

size_t UploadBufferFunc(struct Rend *rend, char **data)
{
  TempUInt glHandle;
  char *buffer;
  size_t bufferSize;
  TempUInt handle;
  TempEnum bufferType = **((TempEnum **)data);
  *data += sizeof(TempEnum);
  handle = **((TempUInt **)data);
  *data += sizeof(TempUInt);
  bufferSize = **((size_t **)data);
  *data += sizeof(size);
  buffer = *data;
  *data += bufferSize;

  glGenBuffers(bufferType, &glHandle);
  SetGLHandle((struct GLRend *)(rend->rend), handle, glHandle);

  glBindBuffer(bufferType, glHandle);

  glBufferData(bufferType, bufferSize, buffer, GL_STATIC_DRAW);
}
void UploadBuffer(struct QueueData *queueData, TempEnum bufferType, TempUInt handle, size_t bufferSize, char *buffer)
{
  AppendParameter(queueData, &bufferType, sizeof(TempEnum));
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendParameter(queueData, &bufferSize, sizeof(TempUInt));
  AppendParameter(queueData, buffer, bufferSize);
  AppendCommand(queueData,&UploadBufferFunc);
}

void AttribPointersFunc(struct Rend *rend, char *data)
{
  size_t stride = *((size_t *)data);
  TempUInt nAttributes = *((TempUInt *)data);
  TempUInt *indicesLensAndOffsets
}
void AttribPointers(struct QueueData *queueData, size_t stride, TempUInt nAttributes, TempUInt *indicesLensAndOffsets)
{
  AppendParameter(queueData, &stride, sizeof(size_t));
  AppendParameter(queueData, &nAttributes, sizeof(nAttributes));
  AppendParameter(queueData, indicesLensAndOffsets, sizeof(TempUInt)*nAttributes*3);
  AppendCommand(queueData,&AttribPointersFunc);
}

void LoadProgramFunc(struct Rend *rend, char *data)
{
  TempUInt handle = *((TempUInt *)data);
  size_t vShaderLen = *((size_t *)data);
  size_t fShaderLen = *((size_t *)data);
  char *vShader, char *fShader
}
void LoadProgram(struct QueueData *queueData, TempUInt handle, size_t vShaderLen, size_t fShaderLen, char *vShader, char *fShader)
{
  AppendParameter(queueData, &vShaderLen, sizeof(size_t));
  AppendParameter(queueData, &fShaderLen, sizeof(size_t));
  AppendParameter(queueData, vShader, vShaderLen * sizeof(char));
  AppendParameter(queueData, fShader, fShaderLen * sizeof(char));
  AppendCommand(queueData,&LoadProgramFunc);
}
