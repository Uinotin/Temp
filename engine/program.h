#ifndef PROGRAM_H
#define PROGRAM_H

#include "commands.h"

typedef struct
{
  TempUInt *program;
} ProgramPtr;

void GenProgramsFunc(char **data);
void GenPrograms(QueueData *queueData, TempSizei nPrograms, ProgramPtr *programPtr);

void LoadProgramFunc(char **data);
void LoadProgram(QueueData *queueData, ProgramPtr *programPtr, size_t vShaderLen, size_t fShaderLen, char *vShader, char *fShader);

void UseProgramFunc(char **data);
void UseProgram(QueueData *queueData, TempUInt handle);


#endif
