#ifndef GLREND_H
#define GLREND_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "commandqueue.h"

struct GLRend
{
  struct CommandQueue commandQueue;

  GLuint *handles;
  unsigned int nHandles;
};

void InitRendGL(struct GLRend *rend, unsigned int nHandles);

struct CommandQueue *GetRendCommandQueueGL(struct GLRend *rend);

void DrawFrameGL(struct GLRend *rend);

GLuint GetGLHandle(struct GLRend *rend, unsigned int handle);
void SetGLHandle(struct GLRend *rend, unsigned int handle, GLuint GLhandle);


#endif
