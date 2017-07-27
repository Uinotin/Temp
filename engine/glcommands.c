#include "commands.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void AppendParameter(QueueData *queueData, void *parameter, size_t len)
{
  assert(queueData->queueLen + len < queueData->queueSize);
  //FIXME: This might cause performance issues if used with non-array parametres?
  memcpy(queueData->queue + queueData->queueLen, parameter, len);
  queueData->queueLen += len;
}

void AppendCommand(QueueData *queueData, Command command)
{
  assert(queueData->nCommands < queueData->maxCommands);
  queueData->commands[(queueData->nCommands++)] = command; 
}


void ClearFunc(char **data)
{
  GLenum bitEnum = *((GLenum *)(*data));
  glClear(bitEnum);
  *data += sizeof(GLenum);
}
		       
void Clear(QueueData *queueData, TempEnum bitEnum)
{
  AppendParameter(queueData, &bitEnum, sizeof(TempEnum));
  AppendCommand(queueData, &ClearFunc);
}

void BindVAOFunc(char **data)
{
  glBindVertexArray(*((TempUInt *)(*data)));
  *data += sizeof(TempUInt);
}
void BindVAO(QueueData *queueData, TempUInt handle)
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
  stride = *((size_t *)(*data));
  *data += sizeof(size_t);
  nAttributes = *((TempUInt *)(*data));
  *data += sizeof(TempUInt);
  indicesLensAndOffsets = (TempUInt *)(*data);
  *data += sizeof(TempUInt)*nAttributes*3;

  for (i = 0; i < nAttributes; ++i)
  {
    glVertexAttribPointer(indicesLensAndOffsets[0], indicesLensAndOffsets[1], GL_FLOAT, GL_FALSE, stride, (char*)0 + indicesLensAndOffsets[2]);
    glEnableVertexAttribArray(indicesLensAndOffsets[0]);
    indicesLensAndOffsets += 3;
  }
}
void AttribPointers(QueueData *queueData, size_t stride, TempUInt nAttributes, TempUInt *indicesLensAndOffsets)
{
  AppendParameter(queueData, &stride, sizeof(size_t));
  AppendParameter(queueData, &nAttributes, sizeof(nAttributes));
  AppendParameter(queueData, indicesLensAndOffsets, sizeof(TempUInt)*nAttributes*3);
  AppendCommand(queueData,&AttribPointersFunc);
}

static GLuint CompileShader(const GLchar **shaderSource, GLenum shaderType, GLint len)
{
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, shaderSource, &len);
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
  return shader;
}

void LoadProgramFunc(char **data)
{
  const TempChar *vShader, *fShader;
  TempUInt handle, vHandle, fHandle;
  size_t vShaderLen, fShaderLen;
  handle = *((TempUInt *)(*data));
  (*data) += sizeof(TempUInt);
  vShaderLen = *((size_t *)(*data));
  (*data) += sizeof(size_t);
  fShaderLen = *((size_t *)(*data));
  (*data) += sizeof(size_t);
  vShader = (const TempChar *)(*data);
  (*data) += sizeof(TempChar) * vShaderLen;
  fShader = (const TempChar *)(*data);
  (*data) += sizeof(TempChar) * fShaderLen;


  vHandle = CompileShader(((const GLchar **)&vShader), GL_VERTEX_SHADER, (GLint)vShaderLen);
  fHandle = CompileShader(((const GLchar **)&fShader), GL_FRAGMENT_SHADER, (GLint)fShaderLen);
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  glLinkProgram(handle);
}
void LoadProgram(QueueData *queueData, TempUInt handle, size_t vShaderLen, size_t fShaderLen, TempChar *vShader, TempChar *fShader)
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
  glUseProgram(*((TempUInt *)(*data)));
  *data += sizeof(TempUInt);
}

void UseProgram(QueueData *queueData, TempUInt handle)
{
  AppendParameter(queueData, &handle, sizeof(TempUInt));
  AppendCommand(queueData,&UseProgramFunc);
}

void DrawArraysFunc(char **data)
{
  TempEnum drawType;
  TempInt start;
  TempSizei len;
  drawType = *((TempEnum *)(*data));
  *data += sizeof(TempEnum);
  start = *((TempInt *)(*data));
  *data += sizeof(TempInt);
  len = *((TempSizei *)(*data));
  *data += sizeof(TempSizei);
  glDrawArrays(drawType, start, len);
}

void DrawArrays(QueueData *queueData, TempEnum drawType, TempInt start, TempSizei len)
{
  AppendParameter(queueData, &drawType, sizeof(TempEnum));
  AppendParameter(queueData, &start, sizeof(TempInt));
  AppendParameter(queueData, &len, sizeof(TempSizei));
  AppendCommand(queueData, &DrawArraysFunc);
}
