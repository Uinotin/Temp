#ifndef GLREND_H
#define GLREND_H
#include <pthread.h>
#include "commandqueue.h"
#include "buffer.h"

struct Handles
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
};

struct Rend
{
  struct CommandQueueList commandQueueList;
  struct CommandQueue defaultCommandQueue;

  struct Handles handles;
};

void InitRend(struct Rend *rend);

void DestroyRend(struct Rend *rend);

void GenHandlesFunc(char **data);

void GenHandles(struct QueueData *queueData, struct Handles *handles);

void AllocHandles(struct Rend *rend, unsigned int nBuffers, unsigned int nPrograms, unsigned int nVAOs);

TempUInt GetBufferHandle(struct Rend *rend);

TempUInt GetProgramHandle(struct Rend *rend);

TempUInt GetVAOHandle(struct Rend *rend);

void ExecCommands(struct Rend *rend);

void StartAppend(struct Rend *rend);
void Append(struct Rend *rend, struct CommandQueue *commandQueue);

void FinishAppend(struct Rend *rend);

struct CommandQueue *GetFirstCommandQueue(struct Rend *rend);

void SetFirstCommandQueue(struct Rend *rend, struct CommandQueue *commandQueue);

void CreateBuffer(struct Rend *rend, struct Buffer *buffer, TempEnum bufferType, size_t bufferSize);

void SyncThreads(void);

#endif
