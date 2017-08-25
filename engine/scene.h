#ifndef SCENE_H
#define SCENE_H

#include "typedefs.h"

typedef void (*UpdateFunc)(char *modulePointer, double deltaTime);
typedef void (*DrawFunc)(char *modulePointer);
typedef void (*KeyFunc)(char *modulePointer, int key, int action);

struct Scene
{
  TempInt sceneType;
  void (*Init[10])(struct Scene *scene, TempInt currentIndex);
  TempSizei (*Alloc[10])(struct Scene *scene, TempInt currentIndex);
  void (*SetVals[10])(struct Scene *scene, TempInt currentIndex);
  UpdateFunc Update[10];
  KeyFunc Key[10];
  
  DrawFunc Upload[10];
  DrawFunc Draw[10];
  DrawFunc UpdateDrawData[10];

  char *drawDataPointers[10];
  TempSizei nDrawables;

  char *modulePointers[10];
  TempSizei modulePointersSize;

  char *dataPointer;
  TempSizei usedData;
  TempSizei dataPointerSize;

  TempSizei nModules;
};

typedef struct Scene Scene;

typedef void (*InitFunc)(Scene *scene, TempInt currentIndex);
typedef TempSizei (*AllocFunc)(Scene *scene, TempInt currentIndex); 

void InitScene(Scene *scene);
void LoadScene(Scene *scene);
void UpdateScene(Scene *scene, double deltaTime);
void DrawScene(Scene *scene);
void UpdateSceneDrawData(Scene *scene);
void SceneKey(Scene *scene, int key, int action);

char *GetDataPointer(Scene *scene, TempSizei size);

#endif
