#ifndef COMMANDQUEUELIST_H
#define COMMANDQUEUELIST_H

#include "commandqueue.h"

struct CommandQueueList
{
  struct CommandQueueList *twin;
  CommandQueue *first;
  CommandQueue *last;
  
  pthread_mutex_t mutex;
};

typedef struct CommandQueueList CommandQueueList;

void StartCommandQueueList(CommandQueueList *commandQueueList);
void LockCommandQueueList(CommandQueueList *commandQueueList);
void UnlockCommandQueueList(CommandQueueList *commandQueueList);
void StartAppend(CommandQueueList *commandQueueList);
void Append(CommandQueueList *commandQueueList, CommandQueue *commandQueue);
void FinishAppend(CommandQueueList *commandQueueList);
void EmptyCommandQueueList(CommandQueueList *commandQueueList);
void DestroyCommandQueueList(CommandQueueList *commandQueueList);

#endif
