#ifndef GLREND_H
#define GLREND_H
#include <pthread.h>
#include "commandqueuelist.h"
#include "buffer.h"

typedef struct 
{
  CommandQueueList commandQueueList;
} Rend;

void InitRend(Rend *rend);

void DestroyRend(Rend *rend);

void ExecCommands(Rend *rend);

void SyncThreads(void);

#endif
