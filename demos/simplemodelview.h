#ifndef SIMPLEMODELVIEW_H
#define SIMPLEMODELVIEW_H

#include "../engine/buffers.h"

typedef struct
{
  TempUInt *handles;
  TempEnum *types;
  TempUInt VAO;
  TempUInt program;
  TempSizei vertCount;
  TempSizei indxCount;
  
} SimpleModelView;

void AddSimpleModelView(Scene *scene);
void InitSimpleModelView(Scene *scene, TempInt currentIndex);
TempSizei AllocSimpleModelView(Scene *scene, TempInt currentIndex);
void SetSimpleModelViewVals(Scene *scene, TempInt currentIndex);
void UploadSimpleModelView(char *passTypePointer);
void UpdateSimpleModelView(char *passTypePointer, float deltaTime);
void DrawSimpleModelView(char *passTypePointer);
  

#endif
