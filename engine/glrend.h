#ifndef GLREND_H
#define GLREND_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <pthread.h>
#include "commandqueue.h"

struct GLRend
{
  struct CommandQueue *firstCommandQueue;
  struct CommandQueue *lastCommandQueue;
  ptread_mutex_t commandQueueListMutex;
  

  GLuint *handles;
  unsigned int nHandles;
};

void InitRendGL(struct GLRend *rend, unsigned int nHandles);
void DestroyRendGL(struct GLRend *rend);

void ExecCommands(struct GLRend *rend);

void AppendCommandQueue(struct GLRend *rend);

GLuint GetGLHandle(struct GLRend *rend, unsigned int handle);
void SetGLHandle(struct GLRend *rend, unsigned int handle, GLuint GLhandle);


#endif
