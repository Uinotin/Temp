#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <pthread.h>
#include "typedefs.h"

struct QueueData
{
  TempChar *queue;
  unsigned int queueLen;
  unsigned int queueArraySize;
  Command *commands;
  unsigned int nCommands;
  unsigned int maxCommands;
};

struct CommandQueue
{
  struct QueueData queueData;

  struct CommandQueue *prev;
  struct CommandQueue *next;

  pthread_mutex_t mutex;
};

void StartCommandQueue(struct CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize);
void EmptyCommandQueue(struct CommandQueue *commandQueue);
void AppendCommand(struct CommandQueue *dest, struct QueueData *source);

void LockCommandQueue(struct CommandQueue *commandQueue);
void UnlockCommandQueue(struct CommandQueue *commandQueue);

#endif
