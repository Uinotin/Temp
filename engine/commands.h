#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"
#include "commandqueue.h"

void AppendParameter(QueueData *queueData, void *parameter, size_t len);
void AppendCommand(QueueData *queueData, Command command);

void ClearFunc(char **data);
void Clear(QueueData *queueData, TempEnum bitEnum);

void BindVAOFunc(char **data);
void BindVAO(QueueData *queueData, TempUInt handle);

void AttribPointersFunc(char **data);
void AttribPointers(QueueData *queueData, size_t stride, TempUInt nAttributes, TempUInt *indicesLensAndOffsets);

void LoadProgramFunc(char **data);
void LoadProgram(QueueData *queueData, TempUInt handle, size_t vShaderLen, size_t fShaderLen, char *vShader, char *fShader);

void UseProgramFunc(char **data);
void UseProgram(QueueData *queueData, TempUInt handle);

void DrawArraysFunc(char **data);
void DrawArrays(QueueData *queueData, TempEnum drawType, TempInt start, TempSizei len);
#endif
