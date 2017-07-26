#include <stdlib.h>
#include <stdio.h>
#include "commandqueue.h"


void StartCommandQueue(struct CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize)
{
  commandQueue->mutex = PTHREAD_MUTEX_INITIALIZER;

  commandQueue->queueData.queue = (char *)malloc(queueArraySize);
  commandQueue->queueData.commands = (Command *)malloc(sizeof(Command)*commandsArraySize);
  commandQueue->queueData.queueArraySize = queueArraySize;
  commandQueue->queueData.maxCommands = commandsArraySize;
  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;
  commandQueue->prev = 0;
  commandQueue->next = 0;
}

void EmptyCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_lock(&(commandQueue->mutex));

  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;

  pthread_mutex_unlock(&(commandQueue->mutex));
}

void FreeCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_destroy(&(commandQueue->mutex));
  free(commandQueue->queueData.queue);
  free(commandQueue->queueData.commands);
}

void AppendCommandData(struct CommandQueue *dest, struct QueueData *src)
{
  unsigned int newQueueLen;
  pthread_mutex_lock(&(dest->mutex));

  newQueueLen = dest->queueData.queueLen + src->queueLen;

  if(dest->queueData.queueArraySize < newQueueLen) 
  {
    dest->queueData.queueArraySize = newQueueLen;
    //Inefficiency warning
    dest->queueData.queue = (TempChar *)realloc((void *)dest->queueData.queue, newQueueLen);
  }
  memcpy(dest->queueData.queue + dest->queueData.queueLen, src->queue, src->queueLen);
  dest->queueData.queueLen = newQueueLen;
  pthread_mutex_unlock(&(dest->mutex));
}

void LockCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_lock(&(commandQueue->mutex));
}


void UnlockCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_unlock(&(commandQueue->mutex));
}

void StartCommandQueueList(struct CommandQueueList *commandQueueList)
{
  commandQueueList->first = 0;
  commandQueueList->last = 0;

  commandQueueList->mutex = PTHREAD_MUTEX_INITIALIZER;
}

void LockCommandQueueList(struct CommandQueueList *commandQueueList)
{
  pthread_mutex_lock(&(commandQueueList->mutex));
}

void UnlockCommandQueueList(struct CommandQueueList *commandQueueList)
{
  pthread_mutex_unlock(&(commandQueueList->mutex));
}

void SyncCommandQueueList(struct CommandQueueList *dst, struct CommandQueueList *src)
{
  struct CommandQueue *queue;
  LockCommandQueueList(dst);
  queue = src->first;
  if (queue)
  {
    LockCommandQueue(queue);
    dst->first = src->first->twin;
    dst->last = src->last->twin;
  }
  else
  {
    dst->first = 0;
    dst->last = 0;
  }

  while(queue)
  {
    
  }
}
