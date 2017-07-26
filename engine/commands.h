#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"
#include "commandqueue.h"

void AppendParameter(struct QueueData *queueData, void *parameter, size_t len);
void AppendCommand(struct QueueData *queueData, Command command);

void ClearFunc(char **data);
void Clear(struct QueueData *queueData, TempEnum bitEnum);

void BindVAOFunc(char **data);
void BindVAO(struct QueueData *queueData, TempUInt handle);

void AttribPointersFunc(char **data);
void AttribPointers(struct QueueData *queueData, size_t stride, TempUInt nAttributes, TempUInt *indicesLensAndOffsets);

void LoadProgramFunc(char **data);
void LoadProgram(struct QueueData *queueData, TempUInt handle, size_t vShaderLen, size_t fShaderLen, char *vShader, char *fShader);
#endif
