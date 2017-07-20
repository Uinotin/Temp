#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"
#include "commandqueue.h"

size_t GenBuffer(struct Rend *rend, char *data);
void AppendGenBuffer(struct CommandQueue *commandQueue, TempUInt handle); 
size_t BindBuffer(struct Rend *rend, char *data);
void AppendBindBuffer(struct CommandQueue *commandQueue, TempEnum bufferType, TempUInt handle);

size_t LoadShader(struct Rend *rend, char *data);

#endif
