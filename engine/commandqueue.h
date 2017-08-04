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
  pthread_mutex_t dataMutex;
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

void InitCommandQueue(CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize);
void DestroyCommandQueue(CommandQueue *commandQueue);
void EmptyCommandQueue(CommandQueue *commandQueue);
void AppendCommandData(CommandQueue *dest, QueueData *source);

void LockCommandQueue(CommandQueue *commandQueue);
void UnlockCommandQueue(CommandQueue *commandQueue);
void LockCommandQueueData(QueueData *queueData);
void UnlockCommandQueueData(QueueData *queueData);

#endif
