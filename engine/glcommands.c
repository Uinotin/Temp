#include "commands.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void AppendParameter(QueueData *queueData, void *parameter, size_t len)
{
  assert(queueData->queueLen + len < queueData->queueSize);
  //FIXME: This might cause performance issues if used with non-array parametres?
  memcpy(queueData->queue + queueData->queueLen, parameter, len);
  queueData->queueLen += len;
}

void AppendCommand(QueueData *queueData, Command command)
{
  assert(queueData->nCommands < queueData->maxCommands);
  queueData->commands[(queueData->nCommands++)] = command; 
}


void ClearFunc(char **data)
{
  GLenum bitEnum = *((GLenum *)(*data));
  glClear(bitEnum);
  *data += sizeof(GLenum);
}
		       
void Clear(QueueData *queueData, TempEnum bitEnum)
{
  AppendParameter(queueData, &bitEnum, sizeof(TempEnum));
  AppendCommand(queueData, &ClearFunc);
}


void DrawArraysFunc(char **data)
{
  TempEnum drawType;
  TempInt start;
  TempSizei len;
  drawType = *((TempEnum *)(*data));
  *data += sizeof(TempEnum);
  start = *((TempInt *)(*data));
  *data += sizeof(TempInt);
  len = *((TempSizei *)(*data));
  *data += sizeof(TempSizei);
  glDrawArrays(drawType, start, len);
}

void DrawArrays(QueueData *queueData, TempEnum drawType, TempInt start, TempSizei len)
{
  AppendParameter(queueData, &drawType, sizeof(TempEnum));
  AppendParameter(queueData, &start, sizeof(TempInt));
  AppendParameter(queueData, &len, sizeof(TempSizei));
  AppendCommand(queueData, &DrawArraysFunc);
}
