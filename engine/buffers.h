#ifndef BUFFERS_H
#define BUFFERS_H

#include "scene.h"

typedef struct
{
  char *data[10];
  TempUInt sizes[10];
  TempEnum drawType[10];
  TempEnum type[10];
  TempUInt handle[10];
  TempSizei nBuffers;
  TempUInt buffersVAO;
} Buffers;

void AddBuffers(Scene *scene);
void InitBuffers(Scene *scene, TempInt currentIndex);
TempSizei AllocBuffers(Scene *scene, TempInt currentIndex);
void SetBuffersVals(Scene *scene, TempInt currentIndex);
void UploadBuffers(char *passTypePointer);
void UpdateBuffers(char *passTypePointer, double deltaTime);
void DrawBuffers(char *passTypePointer);
void UpdateBuffersDrawData(char *passTypePointer);
void BuffersKey(char *passTypePointer, int key, int action);

void AllocBuffer(Buffers *buffers, TempEnum bufferType, TempEnum drawType, TempSizei size);
#endif
