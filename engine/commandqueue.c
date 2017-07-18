#include "commandqueue.h"


void StartCommandQueue(struct CommandQueue *commandQueue)
{
  commandQueue->mutex = PTHREAD_MUTEX_INITIALIZER;

  commandQueue->queue = (void *)malloc(255 * sizeof(char));
  commandQueue->queueArraySize = 255;
  commandQueue->queueLen = 0;
}

void EmptyCommandQueue(struct CommandQueue *commandQueue)
{
  pthread_mutex_lock(commandQueue->mutex);

  commandQueue->queueLen = 0;

  pthread_mutex_unlock(commandQueue->mutex);
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
    dest->queueData.queue = (void *)realloc((void *)dest->queueData.queue, newQueueLen);
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
