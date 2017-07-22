#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <pthread.h>
#include "typedefs.h"


//Might be worth exploring the possibility of the CommandQueue -list having an identical twin.

struct QueueData
{
  TempChar *queue;
  unsigned int queueLen;
  unsigned int queueArraySize;
  Command *commands;
  unsigned int nCommands;
  unsigned int maxCommands;
  int isDirty;
};

struct CommandQueue
{
  struct QueueData queueData;

  struct CommandQueue *prev;
  struct CommandQueue *next;
  struct CommandQueue *twin;

  int isWriteOnly;

  pthread_mutex_t mutex;
};

struct CommandQueueList
{
  struct CommandQueueList *twin;
  struct CommandQueue *first;
  struct CommandQueue *last;
  
  pthread_mutex_t mutex;
}

void StartCommandQueue(struct CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize);
void EmptyCommandQueue(struct CommandQueue *commandQueue);
void AppendCommand(struct CommandQueue *dest, struct QueueData *source);

void LockCommandQueue(struct CommandQueue *commandQueue);
void UnlockCommandQueue(struct CommandQueue *commandQueue);

void LockCommandQueueList(struct CommandQueueList *commandQueueList);
void UnlockCommandQueueList(struct CommandQueueList *commandQueueList);

void SyncCommandQueueList(struct CommandQueueList *dst, struct CommandQueueList *src);

#endif
