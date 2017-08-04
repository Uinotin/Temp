#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "typedefs.h"
#include "attrib.h"

typedef struct
{
  TempUInt vaoHandle;
} VertexArray;

typedef struct
{
  TempUInt *vaoHandle;
} VertexArrayPtr;

void GenVAOsFunc(char **data);
void GenVAOs(QueueData *queueData, TempSizei nVertexArrays, VertexArrayPtr *vertexArrayPtr);

void BindVAOPtrFunc(char **data);
void BindVAOPtr(QueueData *queueData, VertexArrayPtr *VAOPtr);

void BindVAOFunc(char **data);
void BindVAO(QueueData *queueData, TempUInt handle);

/*
Attrib AllocAttribPointer(Buffer *buffer, Attrib *attrib);
void FreeAttribPointer(Attrib *attrib);

void BufferSubDataFunc(char **data);
void BufferSubData(QueueData *queueData, Attrib *attrib);
*/

#endif
