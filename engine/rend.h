#ifndef GLREND_H
#define GLREND_H
#include <pthread.h>
#include "commandqueue.h"
#include "buffer.h"

typedef struct 
{
  TempUInt *buffers;
  unsigned int nBuffers;
  unsigned int usedBuffers;

  TempUInt *programs;
  unsigned int nPrograms;
  unsigned int usedPrograms;

  TempUInt *VAOs;
  unsigned int nVAOs;
  unsigned int usedVAOs;
} Handles;

typedef struct 
{
  CommandQueueList commandQueueList;
  CommandQueue defaultCommandQueue;

  Handles handles;
} Rend;

void InitRend(Rend *rend);

void DestroyRend(Rend *rend);

void GenHandlesFunc(char **data);

void GenHandles(QueueData *queueData, Handles *handles);

void AllocHandles(Rend *rend, unsigned int nBuffers, unsigned int nPrograms, unsigned int nVAOs);

TempUInt GetBufferHandle(Rend *rend);

TempUInt GetProgramHandle(Rend *rend);

TempUInt GetVAOHandle(Rend *rend);

void ExecCommands(Rend *rend);

void StartAppend(Rend *rend);
void Append(Rend *rend, CommandQueue *commandQueue);

void FinishAppend(Rend *rend);

CommandQueue *GetFirstCommandQueue(Rend *rend);

void SetFirstCommandQueue(Rend *rend, CommandQueue *commandQueue);

void CreateBuffer(Rend *rend, Buffer *buffer, TempEnum bufferType, size_t bufferSize);

void SyncThreads(void);

#endif
