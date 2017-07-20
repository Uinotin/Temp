#ifndef REND_H
#define REND_H

//Mimicing an interface. Not sure if this is a good idea.
struct Rend
{
  void *rend;
};

void InitRend(struct Rend *rend, unsigned int nHandles);

void DestroyRend(struct Rend *rend);

void ExecCommands(struct Rend *rend);

#endif
