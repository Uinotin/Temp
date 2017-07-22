#ifndef GLREND_H
#define GLREND_H
#include <pthread.h>
#include "commandqueue.h"

struct Rend
{
  struct CommandQueueList commandQueueList;

  TempUInt *handles;
  unsigned int nHandles;
};

void InitRend(struct Rend *rend, unsigned int nHandles);
void DestroyRend(struct Rend *rend);

void ExecCommands(struct Rend *rend);

void AppendCommandQueue(struct Rend *rend);

TempUInt GetGLHandle(struct Rend *rend, unsigned int handle);
void SetGLHandle(struct Rend *rend, unsigned int handle, TempUInt GLhandle);


#endif
