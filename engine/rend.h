#ifndef REND_H
#define REND_H

//Mimicing an interface. Not sure if this is a good idea.
struct Rend
{
  void *rend;
};

void InitRend(struct Rend *rend, unsigned int nHandles);

struct CommandQueue *GetRendCommandQueue(struct Rend *rend);

void DrawFrame(struct Rend *rend);

#endif
