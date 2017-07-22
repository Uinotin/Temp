#include "glrend.h"
#include "commands.h"

void InitRend(struct Rend *rend, unsigned int nHandles)
{
  rend->nHandles = nHandles;
  rend->handles = (GLuint *)malloc(nHandles*sizeof(GLuint));


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



void ExecCommands(struct Rend *rend)
{
  struct CommandQueue *commandQueue = rend->commandQueueList.first;
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


void AppendCommandQueue(struct Rend *rend, struct CommandQueue *commandQueue)
{
  struct CommandQueue *prev;
  pthread_mutex_lock(rend->commandQueueList.mutex);
  prev = rend->commandQueueList.last;
  LockCommandQueue(commandQueue);
  if(prev)
    prev->next = commandQueue;
  commandQueue->prev = rend->commandQueueList.last;
  commandQueue->next = 0;
  rend->commandQueueList.last = commandQueue;
  if(prev)
    UnlockCommandQueue(prev);
  else
    rend->commandQueueList.first = commandQueue;
    
  pthread_mutex_unlock(rend->commandQueueList.mutex);
}

void FinishCommands(struct Rend *rend)
{
  UnlockCommandQueue(rend->last);
}

struct CommandQueue *GetFirstCommandQueue(struct Rend *rend)
{
  return rend->commandQueueList.first;
}

void SetFirstCommandQueue(struct Rend *rend, struct CommandQueue *commandQueue)
{
  rend->commandQueueList.first = commandQueue;
}

GLuint GetGLHandle(struct Rend *rend, unsigned int handle)
{
  return rend->handles[handle];
}

void SetGLHandle(struct Rend *rend, unsigned int handle, GLuint GLhandle)
{
  rend->handles[handle] = GLhandle;
}
