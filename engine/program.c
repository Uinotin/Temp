#include <stdlib.h>
#include <stdio.h>
#include "program.h"

void GenProgramsFunc(char **data)
{
  TempSizei i;
  ProgramPtr *programPtr;
  TempSizei size = *((TempSizei *)*data);
  *data += sizeof(TempSizei);
  programPtr = (ProgramPtr *)*data;
  *data += sizeof(ProgramPtr);
  for(i = 0; i < size; ++i)
    programPtr->program[i] = glCreateProgram();
}

void GenPrograms(QueueData *queueData, TempSizei nPrograms, ProgramPtr *programPtr)
{
  programPtr->program = (TempUInt *)malloc(nPrograms * sizeof(TempUInt));
  AppendParameter(queueData, &nPrograms, sizeof(TempSizei));
  AppendParameter(queueData, programPtr, sizeof(ProgramPtr));
  AppendCommand(queueData, &GenProgramsFunc);
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
  TempUInt vHandle, fHandle, handle;
  ProgramPtr *programPtr;
  size_t vShaderLen, fShaderLen;
  programPtr = (ProgramPtr *)(*data);
  (*data) += sizeof(ProgramPtr);
  vShaderLen = *((size_t *)(*data));
  (*data) += sizeof(size_t);
  fShaderLen = *((size_t *)(*data));
  (*data) += sizeof(size_t);
  vShader = (const TempChar *)(*data);
  (*data) += sizeof(TempChar) * vShaderLen;
  fShader = (const TempChar *)(*data);
  (*data) += sizeof(TempChar) * fShaderLen;

  handle = *(programPtr->program);
  vHandle = CompileShader(((const GLchar **)&vShader), GL_VERTEX_SHADER, (GLint)vShaderLen);
  fHandle = CompileShader(((const GLchar **)&fShader), GL_FRAGMENT_SHADER, (GLint)fShaderLen);
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  glLinkProgram(handle);
}
void LoadProgram(QueueData *queueData, ProgramPtr *programPtr, size_t vShaderLen, size_t fShaderLen, TempChar *vShader, TempChar *fShader)
{
  AppendParameter(queueData, programPtr, sizeof(ProgramPtr));
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
