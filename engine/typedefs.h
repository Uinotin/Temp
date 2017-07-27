#ifndef TYPEDEFS_H
#define TYPEDEFS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef _DEBUG
#define OPENGL_DEBUG_OUTPUT 1
#endif

typedef void (*Command)(char **);

typedef GLfloat TempFloat;
typedef GLuint TempUInt;
typedef GLint TempInt;
typedef GLchar TempChar;
typedef GLsizei TempSizei;

enum tempEnum
{
  TEMP_TRIANGLES = GL_TRIANGLES,
  TEMP_ARRAY_BUFFER = GL_ARRAY_BUFFER,
  TEMP_COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT
};

typedef enum tempEnum TempEnum;

#endif
