#include "glrend.h"
#include "commands.h"

void InitRendGL(struct GLRend *rend, unsigned int nHandles)
{
  rend->nHandles = nHandles;
  rend->handles = (GLuint *)malloc(nHandles*sizeof(GLuint));

  rend->firstCommandQueue = 0;
  rend->lastCommandQueue = 0;

  rend->commandQueueListMutex = PTHREAD_MUTEX_INITIALIZER;

  {
    GLenum err;
    glewExperimental = GL_TRUE;
    err = glewInit();
    if (GLEW_OK != err)
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); 
    }
  }
}



void ExecCommandsGL(struct GLRend *rend)
{
  struct CommandQueue *commandQueue = rend->firstCommandQueue;
  while (commandQueue)
  {
    char *queue;
    Command *commands;
    unsigned int j, queueLen;
    queue = commandQueue->queue;
      
    nCommands = commandQueue->queueData.nCommands;
    for (j = 0; j < nCommands; ++j)
    {
      unsigned int currentCommandSize = sizeof(Command) + (*(commands)(rend, (void *)(queue + sizeof(Command))));
      commands += sizeof(Command);
      queue += currentCommandSize;
    }
    LockCommandQueue(commandQueue);
    commandQueue = commandQueue->next;
    UnlockCommandQueue(commandQueue->prev);
  }
}


void AppendCommandQueue(struct GLRend *rend, struct CommandQueue *commandQueue)
{
  struct CommandQueue *prev;
  pthread_mutex_lock(rend->commandQueueListMutex);
  prev = rend->lastCommandQueue;
  LockCommandQueue(commandQueue);
  if(prev)
    prev->next = commandQueue;
  commandQueue->prev = rend->lastCommandQueue;
  commandQueue->next = 0;
  rend->lastCommandQueue = commandQueue;
  if(prev)
    UnlockCommandQueue(prev);
  else
    rend->firstCommandQueue = commandQueue;
    
  pthread_mutex_unlock(rend->commandQueueListMutex);
}

void FinishCommands(struct GLRend *rend)
{
  UnlockCommandQueue(rend->last);
}

struct CommandQueue *GetFirstCommandQueue(struct GLRend *rend)
{
  return rend->firstCommandQueue;
}

void SetFirstCommandQueue(struct GLRend *rend, struct CommandQueue *commandQueue)
{
  rend->firstCommandQueue = commandQueue;
}

GLuint GetGLHandle(struct GLRend *rend, unsigned int handle)
{
  return rend->handles[handle];
}

void SetGLHandle(struct GLRend *rend, unsigned int handle, GLuint GLhandle)
{
  rend->handles[handle] = GLhandle;
}
