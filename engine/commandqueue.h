#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <pthread.h>
#include "commands.h"

struct QueueData
{
  char *queue;
  unsigned int queueLen;
};

struct CommandQueue
{
  struct QueueData queueData;
  unsigned int queueArraySize;

  pthread_mutex_t mutex;
};

void StartCommandQueue(struct CommandQueue *commandQueue);
void EmptyCommandQueue(struct CommandQueue *commandQueue);
void AppendCommand(struct CommandQueue *dest, struct QueueData *source);

void LockCommandQueue(struct CommandQueue *commandQueue);
void UnlockCommandQueue(struct CommandQueue *commandQueue);

#endif
