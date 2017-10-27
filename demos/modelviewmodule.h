#ifndef MODELVIEWMODULE_H
#define MODELVIEWMODULE_H

#include "../engine/programtree.h"

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
void DrawModelView(ProgramTreeNode *drawNode);
void FreeDrawModelView(ProgramTreeNode *drawNode);

#endif
