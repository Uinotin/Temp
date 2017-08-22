#include <stdlib.h>
#include "scene.h"

void InitScene(Scene *scene)
{
  scene->nModules = 0;
  scene->nDrawables = 0;
  scene->modulePointersSize = 0;
  scene->dataPointerSize = 0;
  scene->usedData = 0;
  scene->sceneType = 0;
}

void LoadScene(Scene *scene)
{
  TempSizei dataPointerSize;
  int i, nModules = scene->nModules;
  dataPointerSize = 0;
  *(scene->modulePointers) = (char *)malloc(scene->modulePointersSize);
  for(i = 0; i < nModules; ++i)
    scene->Init[i](scene, i);

  for(i = 0; i < nModules; ++i)
    dataPointerSize += scene->Alloc[i](scene, i);

  scene->dataPointerSize = dataPointerSize;
  scene->dataPointer = (char *)malloc(dataPointerSize);

  for(i = nModules - 1; i >= 0; --i)
    scene->SetVals[i](scene, i);

  nModules = scene->nDrawables;
  
  for(i = 0; i < nModules; ++i)
    scene->Upload[i](scene->drawDataPointers[i]);
}

void UpdateScene(Scene *scene, float deltaTime)
{
  int i, nModules = scene->nModules;
  for(i = 0; i < nModules; ++i)
    scene->Update[i](scene->modulePointers[i], deltaTime);
}

void DrawScene(Scene *scene)
{
  int i, nDrawables = scene->nDrawables;
  for(i = 0; i < nDrawables; ++i)
    scene->Draw[i](scene->drawDataPointers[i]);
}

char *GetDataPointer(Scene *scene, TempSizei size)
{
  char *pointer = scene->dataPointer + scene->usedData;
  scene->usedData += size;
  return pointer;
}
