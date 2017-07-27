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
  Handles *handles = &(rend->handles);
  handles->usedPrograms = 0;
  handles->usedBuffers = 0;
  handles->usedVAOs = 0;

  handles->programs = 0;
  handles->buffers = 0;
  handles->VAOs = 0;

  StartCommandQueue(&(rend->defaultCommandQueue), 2, sizeof(TempEnum) + sizeof(Handles));
  Clear(&(rend->defaultCommandQueue.queueData), TEMP_COLOR_BUFFER_BIT);
  rend->commandQueueList.first = &(rend->defaultCommandQueue);
  rend->commandQueueList.last = &(rend->defaultCommandQueue);
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
  Handles *handles = &(rend->handles);
  if(handles->buffers)
  {
    unsigned int i, size;
    glDeleteBuffers(handles->nBuffers, handles->buffers); 
    glDeleteVertexArrays(handles->nVAOs, handles->VAOs);
    size = handles->nPrograms;
    for(i = 0; i < size; ++i)
      glDeleteProgram(handles->programs[i]);
    free(handles->buffers);
    free(handles->VAOs);
    free(handles->programs);
  }
}

void GenHandlesFunc(char **data)
{
  unsigned int i, size;
  Handles *handles = (Handles *)*data;
  glGenBuffers(handles->nBuffers, handles->buffers);
  size = handles->nPrograms;
  for(i = 0; i < size; ++i)
    handles->programs[i] = glCreateProgram();
  glGenVertexArrays(handles->nVAOs, handles->VAOs);
  *data += sizeof(Handles);
}

void GenHandles(QueueData *queueData, Handles *handles)
{
  AppendParameter(queueData, handles, sizeof(Handles));
  AppendCommand(queueData, &GenHandlesFunc);
}

void AllocHandles(Rend *rend, unsigned int nBuffers, unsigned int nPrograms, unsigned int nVAOs)
{
  Handles *handles = &(rend->handles);
  handles->buffers = (TempUInt *)malloc(nBuffers*sizeof(TempUInt));
  handles->programs = (TempUInt *)malloc(nPrograms*sizeof(TempUInt));
  handles->VAOs = (TempUInt *)malloc(nVAOs*sizeof(TempUInt));
  handles->nBuffers = nBuffers;
  handles->nPrograms = nPrograms;
  handles->nVAOs = nVAOs;
  SyncThreads();
  GenHandles(&(rend->defaultCommandQueue.queueData), handles);
  SyncThreads();
  SyncThreads();
  EmptyCommandQueue(&(rend->defaultCommandQueue));
  Clear(&(rend->defaultCommandQueue.queueData), TEMP_COLOR_BUFFER_BIT);
  SyncThreads();
}

TempUInt GetBufferHandle(Rend *rend)
{
  Handles *handles = &(rend->handles);
  assert(buffers && handles->usedBuffers < handles->nBuffers);
  return handles->buffers[handles->usedBuffers++];
}

TempUInt GetProgramHandle(Rend *rend)
{
  Handles *handles = &(rend->handles);
  assert(programs && handles->usedPrograms < handles->nPrograms);
  return handles->programs[handles->usedPrograms++];
}

TempUInt GetVAOHandle(Rend *rend)
{
  Handles *handles = &(rend->handles);
  assert(handles->VAOs && handles->usedVAOs < handles->nVAOs);
  return handles->VAOs[handles->usedVAOs++];
}

void ExecCommands(Rend *rend)
{
  CommandQueue *commandQueue = rend->commandQueueList.first;
  
  while (commandQueue)
  {
    char *queue;
    Command *commands;
    unsigned int j, nCommands;
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
      UnlockCommandQueue(prevCommandQueue);
    }
  }
}

void StartAppend(Rend *rend)
{
  LockCommandQueue(rend->commandQueueList.last);
}
void Append(Rend *rend, CommandQueue *commandQueue)
{
  CommandQueue *prev;
  pthread_mutex_lock(&(rend->commandQueueList.mutex));
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
    
  pthread_mutex_unlock(&(rend->commandQueueList.mutex));
}

void FinishAppend(Rend *rend)
{
  UnlockCommandQueue(rend->commandQueueList.last);
}

CommandQueue *GetFirstCommandQueue(Rend *rend)
{
  return rend->commandQueueList.first;
}

void SetFirstCommandQueue(Rend *rend, CommandQueue *commandQueue)
{
  rend->commandQueueList.first = commandQueue;
}

void CreateBuffer(Rend *rend, Buffer *buffer, TempEnum bufferType, size_t bufferSize)
{
  buffer->data = (char *)malloc(bufferSize);
  buffer->size = bufferSize;
  buffer->handleData.type = bufferType;
  buffer->handleData.handle = GetBufferHandle(rend);
}

void SyncThreads(void)
{
  pthread_barrier_wait(&barrier);
}
