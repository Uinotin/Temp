#ifndef MODELVIEWMODULE_H
#define MODELVIEWMODULE_H

#include "programtree.h"

typedef struct
{
  TempEnum type;
  TempSizei arraySize;
  TempInt offset;
} Attrib;

typedef struct
{
  Attrib attrib;
  TempUInt index;
  TempInt size;
  TempSizei stride;

} VertAttrib;

typedef struct
{
  TempUInt shaderHandle,
    projMatLoc, viewMatLoc, worldMatLoc, normalMatLoc,
    lightPosLoc,
    texLoc, normLoc,
    VAO, VBO, IBO,
    texture, normalMap;
  TempInt vertCount, indxCount;

  TempFloat projMat[16], worldMat[16], viewMat[16], normalMat[9], lightPosition[3];
} DrawModelViewData;


void LoadDrawModelView(ProgramTreeNode *drawNode);  
void DrawModelView(void *locals, void *out);
void FreeDrawModelView(ProgramTreeNode *drawNode);

#endif
