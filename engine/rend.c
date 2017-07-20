#include "rend.h"
#include "glrend.h"

void InitRend(struct Rend *rend, unsigned int nHandles)
{
  struct GLRend *glRend = (struct GLRend *)malloc(sizeof(struct GLRend));

  InitRendGL(glRend, nHandles);

  rend->rend = glRend;
}

void DestroyRend(struct Rend *rend)
{
  DestroyRendGL((struct GLRend *)(rend->rend));
  free(rend->rend);
}

struct CommandQueue **GetRendCommandQueues(struct Rend *rend)
{
  return GetRendCommandQueuesGL((struct GLRend *)rend->rend);
}

void ExecCommands(struct Rend *rend)
{
  ExecCommandsGL((struct GLRend *)rend->rend);
}

unsigned int GenHandle(struct Rend *rend)
{
  return GenHandleGL((struct GLRend *)rend->rend);
}
