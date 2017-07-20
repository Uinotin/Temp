#include "commands.h"
#include "glrend.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

size_t GenBuffer(struct Rend *rend, char *data)
{
  TempUInt glHandle;
  glGenBuffers(1, &glHandle);
  SetGLHandle((struct GLRend *)(rend->rend), *((TempUInt*)data), glHandle);
  return sizeof(TempUInt);
}

void AppendGenBuffer(struct CommandQueue *commandQueue, TempUInt handle)
{
  //assert(commandQueue->commandData.queueLen + sizeof(Command) + sizeof (TempUInt) < commandQueue->queueSize);
  commandQueue->queueData.queue 
}

size_t BindBuffer(struct Rend *rend, char *data)
{
  GLenum bufferEnum = *((GLenum *)data);
  data += sizeof(GLenum);
  glBindBuffer(bufferEnum, GetGLHandle((struct GLRend *)(rend->rend), *((TempUInt *)data)));
  return sizeof(GLenum) + sizeof(TempUInt);
}

size_t LoadShader(struct Rend *rend, char *data)
{
  FILE *file;
  const int stringSize = 1024;
  GLchar fileString[1024];
  GLuint shader;
  int stringIndex;
  const char *pathName = "shader/\0";

  
  
  strcpy(fileString, pathName); 
  strcpy(fileString + strlen(pathName), filename);
  stringIndex = 0;
  shader = 0;
  
  file = fopen(fileString, "r");
  if (file==NULL)
    printf("Could not open shader file %s\n", fileString);
  else
  {
    do {
      assert(stringIndex < stringSize);
      fileString[stringIndex] = fgetc (file);
    } while (fileString[stringIndex++] != EOF);
    fileString[--stringIndex] = 0;
    fclose (file);
  }
  
  if (stringIndex)
  {
    GLchar *fileStringPointer = fileString;
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar **)&fileStringPointer, NULL);
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
  }
  
  return shader;
}
