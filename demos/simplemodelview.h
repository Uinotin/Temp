#ifndef SIMPLEMODELVIEW_H
#define SIMPLEMODELVIEW_H

#include "../engine/buffers.h"

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
  TempUInt viewMatLocation;
  TempFloat viewMat[16];
  TempUInt projMatLocation;
  TempFloat projMat[16];

  TempFloat location[3];
  TempFloat rotAlongY;
  TempFloat rotAlongX;
} SimpleCamera;

typedef struct
{
  TempFloat specIntensity;
  TempFloat specFalloff;
  TempFloat position[3];
  TempUInt lightPosLocation;
} SimpleLight;

typedef struct
{
  TempUInt *handles;
  TempEnum *types;
  TempUInt VAO;
  TempUInt program;
  TempSizei vertCount;
  TempSizei indxCount;
  TempUInt texture;
  TempUInt texUniformHandle;
  TempUInt normalMap;
  TempUInt normalUniformHandle;

  TempFloat rotation;

  TempUInt worldMatLocation;
  TempFloat worldMat[16];
  TempUInt normalMatLocation;
  TempFloat normalMat[9];
  SimpleCamera camera;

  SimpleLight light;

  int *keyDown;
  int upKeyDown;
  int downKeyDown;
  int leftKeyDown;
  int rightKeyDown;
  int aKeyDown;
  int dKeyDown;
  int wKeyDown;
  int sKeyDown;
} SimpleModelView;

void AddSimpleModelView(Scene *scene);
void InitSimpleModelView(Scene *scene, TempInt currentIndex);
TempSizei AllocSimpleModelView(Scene *scene, TempInt currentIndex);
void SetSimpleModelViewVals(Scene *scene, TempInt currentIndex);
void UploadSimpleModelView(char *passTypePointer);
void UpdateSimpleModelView(char *passTypePointer, double deltaTime);
void DrawSimpleModelView(char *passTypePointer);
void UpdateSimpleModelViewDrawData(char *passTypePointer);
void SimpleModelViewKey(char *passTypePointer, int key, int action);

#endif
