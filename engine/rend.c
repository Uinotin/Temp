#include "rend.h"
#include "glrend.h"

void InitRend(struct Rend *rend, unsigned int nHandles)
{
  struct GLRend *glRend = (struct GLRend *)malloc(sizeof(struct GLRend));

  InitRendGL(glRend, nHandles);

  rend->rend = glRend;
}

struct CommandQueue *GetRendCommandQueue(struct Rend *rend)
{
  return GetRendCommandQueueGL((struct GLRend *)rend->rend);
}

void DrawFrame(struct Rend *rend)
{
  DrawFrameGL((struct GLRend *)rend->rend);
}

unsigned int GenHandle(struct Rend *rend)
{
  return GenHandleGL((struct GLRend *)rend->rend);
}
