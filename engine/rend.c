#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rend.h"
#include "commands.h"

pthread_barrier_t barrier;


#if OPENGL_DEBUG_OUTPUT
static void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
  {
    printf("GL_DEBUG_SEVERITY_");
    switch (severity)
    {
      case GL_DEBUG_SEVERITY_HIGH:
	printf("HIGH");
	break;
      case GL_DEBUG_SEVERITY_MEDIUM:
	printf("MEDIUM");
	break;
      case GL_DEBUG_SEVERITY_LOW:
	printf("LOW");
	break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
	printf("NOTIFICATION");
	break;
    }
    printf(":\n%s\n", message);
  }
}
#endif

void InitRend(Rend *rend)
{
  {
    GLenum err;
    glewExperimental = GL_TRUE;
    err = glewInit();
    if (GLEW_OK != err)
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); 
    }
  }

#if OPENGL_DEBUG_OUTPUT
    if(glDebugMessageCallback)
    {
      GLuint unusedIds = 0;
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback((GLDEBUGPROC)openglCallbackFunction, 0);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
			    0,
			    &unusedIds,
			    1);
    }
#endif

  pthread_barrier_init(&barrier, NULL, 2);
}

void DestroyRend(Rend *rend)
{
}

void ExecCommands(Rend *rend)
{
  CommandQueue *commandQueue = rend->commandQueueList.first;
  
  while (commandQueue)
  {
    char *queue;
    Command *commands;
    unsigned int j, nCommands;
    LockCommandQueueData(&(commandQueue->queueData));
    queue = commandQueue->queueData.queue;
    commands = commandQueue->queueData.commands;

    nCommands = commandQueue->queueData.nCommands;
    for (j = 0; j < nCommands; ++j)
      (*(commands++))(&(queue));

    {
      CommandQueue *prevCommandQueue;
      LockCommandQueue(commandQueue);
      prevCommandQueue = commandQueue;
      commandQueue = commandQueue->next;
      UnlockCommandQueueData(&(prevCommandQueue->queueData));
      UnlockCommandQueue(prevCommandQueue);
    }
  }
}

void SyncThreads(void)
{
  pthread_barrier_wait(&barrier);
}
