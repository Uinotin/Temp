#include "commandqueue.h"


void StartCommandQueue(struct CommandQueue *commandQueue, unsigned int commandsArraySize, unsigned int queueArraySize)
{
  commandQueue->mutex = PTHREAD_MUTEX_INITIALIZER;

  commandQueue->queueData.queue = (char *)malloc(queueArraySize);
  commandQueue->queueData.commands = (Command *)malloc(sizeof(Command)*commandsArraySize);
  commandQueue->queueArraySize = queueArraySize;
  commandQueue->commandsArraySize = commandsArraySize;
  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;
  commandQueue->prev = 0;
  commandQueue->next = 0;
}

void EmptyCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_lock(commandQueue->mutex);

  commandQueue->queueData.queueLen = 0;
  commandQueue->queueData.nCommands = 0;

  pthread_mutex_unlock(commandQueue->mutex);
}

void FreeCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_destroy(commandQueue->mutex);
  free(commandQueue->queueData.queue);
  free(commandQueue->queueData.commands);
}

void AppendCommand(struct CommandQueue *dest, struct CommandData *src)
{
  unsigned int newQueueLen;
  pthread_mutex_lock(dest->mutex);

  newQueueLen = dest->queueData.queueLen + src->queueLen;

  if(dest->queueArraySize < newQueueLen) 
  {
    dest->queueArraySize = newQueueLen;
    //Inefficiency warning
    dest->queueData.queue = (TempChar *)realloc((void *)dest->queueData.queue, newQueueLen);
  }
  memcpy(dest->queueData.queue + dest->queueData.queueLen, src->queue, src->queueLen);
  dest->queueData.queueLen = newQueueLen;
  pthread_mutex_unlock(dest->mutex);
}

void LockCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_lock(commandQueue->mutex);
}


void UnlockCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_unlock(commandQueue->mutex);
}
