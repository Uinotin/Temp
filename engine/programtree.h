#ifndef PROGRAMTREE_H
#define PROGRAMTREE_H

#include "typedefs.h"

struct ProgramTreeNode;
struct ProgramTree;

typedef void (*Program)(struct ProgramTreeNode *);

struct ProgramTreeNode
{
  struct ProgramTreeNode **children;
  TempSizei nChildren;
  Program program;
  Program loadProgram;
  Program freeProgram;
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

void StartProgramTree(ProgramTree *programTree, Program rootProgram);
void UpdateProgramTree(ProgramTree *programTree, float deltaTime);

#endif
