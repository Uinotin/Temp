#ifndef PROGRAMTREE_H
#define PROGRAMTREE_H

#include "typedefs.h"

struct ProgramTreeNode;
struct ProgramTree;

typedef void (*Program)(void *, void *);
typedef void (*ResourceProgram)(struct ProgramTreeNode *);

struct ProgramTreeNode
{
  struct ProgramTreeNode **children;
  TempSizei nChildren;
  Program program;
  ResourceProgram loadProgram;
  ResourceProgram freeProgram;
  void *locals;
  void *out;
};

typedef struct ProgramTreeNode ProgramTreeNode;

typedef struct
{
  ProgramTreeNode **currentBranch;
  int *branchesUpdated;

  
  TempSizei nPrograms;
  int maxLevel;
} ProgramTree;

void StartProgramTree(ProgramTree *programTree, ResourceProgram rootProgram);
void UpdateProgramTree(ProgramTree *programTree);

#endif
