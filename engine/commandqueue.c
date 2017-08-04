#include <stdlib.h>
#include <string.h>
#include "commandqueue.h"


void InitCommandQueue(CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize)
{
  pthread_mutex_init(&(commandQueue->mutex), 0);
  pthread_mutex_init(&(commandQueue->queueData.dataMutex), 0);

  commandQueue->queueData.queue = (char *)malloc(queueArraySize);
  commandQueue->queueData.commands = (Command *)malloc(sizeof(Command)*commandsArraySize);
  commandQueue->queueData.queueArraySize = queueArraySize;
  commandQueue->queueData.maxCommands = commandsArraySize;
  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;
  commandQueue->prev = 0;
  commandQueue->next = 0;
}

void EmptyCommandQueue(CommandQueue *commandQueue)
{
  pthread_mutex_lock(&(commandQueue->queueData.dataMutex));

  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;

  pthread_mutex_unlock(&(commandQueue->queueData.dataMutex));
}

void DestroyCommandQueue(CommandQueue *commandQueue)
{
  pthread_mutex_destroy(&(commandQueue->mutex));
  pthread_mutex_destroy(&(commandQueue->queueData.dataMutex));
  free(commandQueue->queueData.queue);
  free(commandQueue->queueData.commands);
}

void AppendCommandData(CommandQueue *dest, QueueData *src)
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

void LockCommandQueue(CommandQueue *commandQueue)
{
  pthread_mutex_lock(&(commandQueue->mutex));
}

void UnlockCommandQueue(CommandQueue *commandQueue)
{
  pthread_mutex_unlock(&(commandQueue->mutex));
}

void LockCommandQueueData(QueueData *queueData)
{
  pthread_mutex_lock(&(queueData->dataMutex));
}

void UnlockCommandQueueData(QueueData *queueData)
{
  pthread_mutex_unlock(&(queueData->dataMutex));
}
