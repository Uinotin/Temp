#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"
#include "commandqueue.h"

void AppendParameter(QueueData *queueData, void *parameter, size_t len);
void AppendCommand(QueueData *queueData, Command command);

void ClearFunc(char **data);
void Clear(QueueData *queueData, TempEnum bitEnum);

void DrawArraysFunc(char **data);
void DrawArrays(QueueData *queueData, TempEnum drawType, TempInt start, TempSizei len);
#endif
