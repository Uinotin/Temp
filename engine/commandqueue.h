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

typedef struct QueueData QueueData;

struct CommandQueue
{
  QueueData queueData;

  struct CommandQueue *prev;
  struct CommandQueue *next;
  struct CommandQueue *twin; 

  int syncWithTwin;

  pthread_mutex_t mutex;
};

typedef struct CommandQueue CommandQueue;

struct CommandQueueList
{
  struct CommandQueueList *twin;
  CommandQueue *first;
  CommandQueue *last;
  
  pthread_mutex_t mutex;
};

typedef struct CommandQueueList CommandQueueList;

void StartCommandQueue(CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize);
void FreeCommandQueue(CommandQueue *commandQueue);
void EmptyCommandQueue(CommandQueue *commandQueue);
void AppendCommandData(CommandQueue *dest, QueueData *source);

void LockCommandQueue(CommandQueue *commandQueue);
void UnlockCommandQueue(CommandQueue *commandQueue);


void StartCommandQueueList(CommandQueueList *commandQueueList);
void LockCommandQueueList(CommandQueueList *commandQueueList);
void UnlockCommandQueueList(CommandQueueList *commandQueueList);

void SyncCommandQueueList(CommandQueueList *dst, CommandQueueList *src);

#endif
