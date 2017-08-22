#include "buffers.h"

void InitBuffers(Scene *scene, TempInt currentIndex)
{
  Buffers *buffers;
  int i;
  for(i = 0; i < scene->nDrawables; ++i)
    if(scene->Draw[i] == &DrawBuffers)
    {
      scene->drawDataPointers[i] = scene->modulePointers[currentIndex];
      break;
    }
  scene->Alloc[currentIndex] = &AllocBuffers;
  scene->SetVals[currentIndex] = &SetBuffersVals;
  scene->Update[currentIndex] = &UpdateBuffers;
  buffers = (Buffers *)scene->modulePointers[currentIndex];
  buffers->nBuffers = 0;
  if(currentIndex + 1 < scene->nModules)
    scene->modulePointers[currentIndex + 1] = scene->modulePointers[currentIndex] + sizeof(Buffers);
}

TempSizei AllocBuffers(Scene *scene, TempInt currentIndex)
{
  int i, nBuffers;
  Buffers *buffers;
  TempSizei size = 0;
  buffers = (Buffers *)scene->modulePointers[currentIndex];

  nBuffers = buffers->nBuffers;
  for(i = 0; i < nBuffers; ++i)
    size += buffers->sizes[i];
  return size;
}

void SetBuffersVals(Scene *scene, TempInt currentIndex)
{
  int i, nBuffers;
  Buffers *buffers = (Buffers *)scene->modulePointers[currentIndex];
  nBuffers = buffers->nBuffers;
  for(i = 0; i < nBuffers; ++i)
    buffers->data[i] = GetDataPointer(scene, buffers->sizes[i]);
}

void UploadBuffers(char *passTypePointer)
{
  Buffers *buffers;
  TempUInt *bufferHandles;
  int i, nBuffers;
  buffers = (Buffers *)passTypePointer;
  nBuffers = buffers->nBuffers;
  bufferHandles = buffers->handle;
  glGenVertexArrays(1, &buffers->buffersVAO);
  glBindVertexArray(buffers->buffersVAO);
  glGenBuffers(nBuffers, bufferHandles);
  for(i = 0; i < nBuffers; ++i)
  {
    GLvoid *data = buffers->data[i];
    glBindBuffer(buffers->type[i], bufferHandles[i]);
    glBufferData(buffers->type[i], buffers->sizes[i], data, buffers->drawType[i]);
  }
  glBindVertexArray(buffers->buffersVAO);
}

void UpdateBuffers(char *passTypePointer, float deltaTime)
{
}

void DrawBuffers(char *passTypePointer)
{
}

void AllocBuffer(Buffers *buffers, TempEnum bufferType, TempEnum drawType, TempSizei size)
{
  int i, nBuffers = buffers->nBuffers;
  for(i = 0; i < nBuffers; ++i)
    if(buffers->type[i] == bufferType && buffers->drawType[i])
    {
      buffers->sizes[i] += size;
      return;
    }
  buffers->sizes[nBuffers] = size;
  buffers->type[nBuffers] = bufferType;
  buffers->drawType[nBuffers] = drawType;
  ++(buffers->nBuffers);
}
