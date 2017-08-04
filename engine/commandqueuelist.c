#include "commandqueuelist.h"


void StartCommandQueueList(CommandQueueList *commandQueueList)
{
  commandQueueList->first = 0;
  commandQueueList->last = 0;

  pthread_mutex_init(&(commandQueueList->mutex), 0);
}

void LockCommandQueueList(CommandQueueList *commandQueueList)
{
  pthread_mutex_lock(&(commandQueueList->mutex));
}

void UnlockCommandQueueList(CommandQueueList *commandQueueList)
{
  pthread_mutex_unlock(&(commandQueueList->mutex));
}

void StartAppend(CommandQueueList *commandQueueList)
{
  if(commandQueueList->last)
    LockCommandQueue(commandQueueList->last);
}
void Append(CommandQueueList *commandQueueList, CommandQueue *commandQueue)
{
  CommandQueue *prev;
  pthread_mutex_lock(&(commandQueueList->mutex));
  prev = commandQueueList->last;
  LockCommandQueue(commandQueue);
  if(prev)
    prev->next = commandQueue;
  commandQueue->prev = commandQueueList->last;
  commandQueue->next = 0;
  commandQueueList->last = commandQueue;
  if(prev)
    UnlockCommandQueue(prev);
  else
    commandQueueList->first = commandQueue;
    
  pthread_mutex_unlock(&(commandQueueList->mutex));
}

void FinishAppend(CommandQueueList *commandQueueList)
{
  if(commandQueueList->last)
    UnlockCommandQueue(commandQueueList->last);
}

void EmptyCommandQueueList(CommandQueueList *commandQueueList)
{
  pthread_mutex_lock(&(commandQueueList->mutex));
  commandQueueList->last = 0;
  commandQueueList->first = 0;
  pthread_mutex_unlock(&(commandQueueList->mutex));
}

void DestroyCommandQueueList(CommandQueueList *commandQueueList)
{
  commandQueueList->first = 0;
  commandQueueList->last = 0;
  pthread_mutex_destroy(&(commandQueueList->mutex));
}
