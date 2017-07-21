#ifndef COMMANDS_H
#define COMMANDS_H

#include "typedefs.h"
#include "commandqueue.h"

void GenBuffer(struct CommandQueue *commandQueue, TempUInt handle); 
void BindBuffer(struct CommandQueue *commandQueue, TempEnum bufferType, TempUInt handle);

#endif
