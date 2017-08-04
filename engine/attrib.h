#ifndef ATTRIB_H
#define ATTRIB_H

#include "commands.h"

typedef struct
{
  TempInt size;
  TempEnum type;
  TempInt offset;
} Attrib;

typedef struct
{
  TempUInt index;
  TempSizei stride;
  Attrib attrib;
} VertexAttrib;

void AttribPointersFunc(char **data);
void AttribPointers(QueueData *queueData, TempSizei nAttribs, VertexAttrib *attrib);

#endif
