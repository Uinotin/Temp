#include "commands.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void AppendParameter(struct QueueData *queueData, void *parameter, size_t len)
{
  assert(queueData->queueLen + len < queueData->queueSize);
  //FIXME: This might cause performance issues if used with non-array parametres?
  memcpy(queueData->queue + queueData->queueLen, parameter, len);
  queueData->queueLen += len;
}

void AppendCommand(struct QueueData *queueData, Command command)
{
  assert(queueData->nCommands < queueData->maxCommands);
  queueData->commands[(queueData->nCommands++)] = command; 
}


void ClearFunc(char **data)
{
  GLenum bitEnum = *((GLenum *)*data);
  glClear(bitEnum);
  *data += sizeof(GLenum);
}
		       
void Clear(struct QueueData *queueData, TempEnum bitEnum)
{
  AppendParameter(queueData, &bitEnum, sizeof(TempEnum));
  AppendCommand(queueData, &ClearFunc);
}

void BindVAOFunc(char **data)
{
  glBindVertexArray(*((TempUInt *)*data));
  *data += sizeof(TempUInt);
}
void BindVAO(struct QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData, &BindVAOFunc);
}

void AttribPointersFunc(char **data)
{
  TempUInt i;
  size_t stride;
  TempUInt nAttributes;
  TempUInt *indicesLensAndOffsets;
  char *dataPointer = *data;
  stride = *((size_t *)dataPointer);
  dataPointer += sizeof(TempUInt);
  nAttributes = *((TempUInt *)dataPointer);
  dataPointer += sizeof(TempUInt);
  indicesLensAndOffsets = (TempUInt *)dataPointer;

  for (i = 0; i < nAttributes; ++i)
  {
    glVertexAttribPointer(indicesLensAndOffsets[0], indicesLensAndOffsets[1], GL_FLOAT, GL_FALSE, stride, (char*)0 + indicesLensAndOffsets[2]);
    indicesLensAndOffsets += 3;
  }

  *data += 2 * sizeof(TempUInt) + sizeof(TempUInt)*nAttributes*3;
}
void AttribPointers(struct QueueData *queueData, size_t stride, TempUInt nAttributes, TempUInt *indicesLensAndOffsets)
{
  AppendParameter(queueData, &stride, sizeof(size_t));
  AppendParameter(queueData, &nAttributes, sizeof(nAttributes));
  AppendParameter(queueData, indicesLensAndOffsets, sizeof(TempUInt)*nAttributes*3);
  AppendCommand(queueData,&AttribPointersFunc);
}

static GLuint CompileShader(const GLchar **shaderSource, GLenum shaderType)
{
  shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, (const GLchar **)&shaderSource, NULL);
  glCompileShader(shader);
  {
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
      GLchar log[1024];
      GLint blen = 0;	
      GLsizei slen = 0;

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);       

      if (blen > 1)
      {
        glGetShaderInfoLog(shader, 1024, &slen, log);
        printf("Failed to compile shader:\n%s\n", log);
      }	
      glDeleteShader(shader);
      shader = 0;
    }
  }
  *data += shader;
}

void LoadProgramFunc(char **data)
{
  TempChar *vShader, *fShader;
  TempUInt handle, vHandle, fHandle;
  size_t vShaderLen, fShaderLen;
  char *dataPointer = *data;
  handle = *((TempUInt *)dataPointer);
  dataPointer += sizeof(TempUInt);
  vShaderLen = *((size_t *)dataPointer);
  dataPointer += sizeof(size_t);
  fShaderLen = *((size_t *)dataPointer);
  dataPointer += sizeof(size_t);
  vShader = (TempChar *)dataPointer;
  dataPointer += sizeof(TempChar) * vShaderLen;
  fShader = (TempChar *)dataPointer;

  vHandle = CompileShader(vShader, GL_VERTEX_SHADER);
  fHandle = CompileShader(fShader, GL_FRAGMENT_SHADER);
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);

  *data += sizeof(TempUInt) + sizeof(size_t) * 2 + sizeof(TempChar) * fShaderLen + sizeof(TempChar) * vShaderLen;
}
void LoadProgram(struct QueueData *queueData, TempUInt handle, size_t vShaderLen, size_t fShaderLen, TempChar *vShader, TempChar *fShader)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendParameter(queueData, &vShaderLen, sizeof(size_t));
  AppendParameter(queueData, &fShaderLen, sizeof(size_t));
  AppendParameter(queueData, vShader, vShaderLen * sizeof(TempChar));
  AppendParameter(queueData, fShader, fShaderLen * sizeof(TempChar));
  AppendCommand(queueData,&LoadProgramFunc);
}

void UseProgramFunc(char **data)
{
  glUseProgram(*((TempUInt *)*data));
  *data += sizeof(TempUInt);
}

void UseProgram(struct QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, handle, sizeof(TempUInt));
  AppendCommand(queueData,&UseProgramFunc);
}
