#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define OPENGL_DEBUG_OUTPUT 1

typedef size_t (*Command)(struct Rend *, char *);

typedef TempFloat GLfloat;
typedef TempUInt GLuint;
typedef TempInt GLint;

enum TempEnum
{
  TEMP_ARRAY_BUFFER = GL_ARRAY_BUFFER,
  TEMP_COLOR_BUFFER_BIT = GL_COLOR_BUFFER_BIT
};
