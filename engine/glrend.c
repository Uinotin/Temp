#include "glrend.h"
#include "glcommands.h"

void InitRendGL(struct GLRend *rend, unsigned int nHandles)
{
  rend->nHandles = nHandles;
  rend->handles = (GLuint *)malloc(nHandles*sizeof(GLuint));
  rend->commandQueue = (struct CommandQueue *)malloc(sizeof(struct CommandQueue));
  StartCommandQueue(rend->commandQueue);
}

struct CommandQueue *GetRendCommandQueueGL(struct GLRend *rend)
{
  return &(rend->commandQueue);
}

void DrawFrameGL(struct GLRend *rend)
{
  LockCommandQueue(rend->commandQueue);

  {
    char *queue;
    unsigned int i, queueLen = rend->commandQueue.queueLen;
    queue = rend->commandQueue.queue;
    while (i <= queueLen)
    {
      unsigned int currentCommandSize = sizeof(Command) + (*((Command)queue)(rend, (void *)(queue + sizeof(Command))));
      queue += currentCommandSize;
      i += currentCommandSize;
    }
  }
  
  UnlockCommandQueue(rend->commandQueue);
}

GLuint GetGLHandle(struct GLRend *rend, unsigned int handle)
{
  return rend->handles[handle];
}

void SetGLHandle(struct GLRend *rend, unsigned int handle, GLuint GLhandle)
{
  rend->handles[handle] = GLhandle;
}
