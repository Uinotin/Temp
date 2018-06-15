#include <stdlib.h>
#include "programtree.h"

void StartProgramTree(ProgramTree *programTree, ResourceProgram rootProgram)
{
  ProgramTreeNode **currentBranch;
  int currentBranchLevel, *branchesLoaded;
  currentBranch = (ProgramTreeNode **)malloc(sizeof(ProgramTreeNode *) * 32); //Magic numbers(tm)
  currentBranch[0] = (ProgramTreeNode *)malloc(sizeof(ProgramTreeNode));
  branchesLoaded = (int *)malloc(sizeof(int)*32);
  for(currentBranchLevel = 0; currentBranchLevel < 32; ++currentBranchLevel)
    branchesLoaded[currentBranchLevel] = 0;
  currentBranchLevel = 0;
  currentBranch[0]->loadProgram = rootProgram;
  while(currentBranchLevel >= 0)
  {
    if(!branchesLoaded[currentBranchLevel])
      (*(currentBranch[currentBranchLevel]->loadProgram))(currentBranch[currentBranchLevel]);

    if(branchesLoaded[currentBranchLevel] == currentBranch[currentBranchLevel]->nChildren)
      branchesLoaded[currentBranchLevel--] = 0;
    else
    {
      currentBranch[currentBranchLevel + 1] = currentBranch[currentBranchLevel]->children[branchesLoaded[currentBranchLevel]];
      ++branchesLoaded[currentBranchLevel++];
    }
  }

  programTree->branchesUpdated = branchesLoaded;
  programTree->currentBranch = currentBranch;
  programTree->nPrograms = 0;
}

void UpdateProgramTree(ProgramTree *programTree)
{
  ProgramTreeNode **currentBranch;
  int *branchesUpdated;
  int currentBranchLevel = 0;
  currentBranch = programTree->currentBranch;
  branchesUpdated = programTree->branchesUpdated;
  for(currentBranchLevel = 0; currentBranchLevel < 32; ++currentBranchLevel)
    branchesUpdated[currentBranchLevel] = 0;
  currentBranchLevel = 0;
  while(currentBranchLevel >= 0)
  {
    if(branchesUpdated[currentBranchLevel] == currentBranch[currentBranchLevel]->nChildren)
    {
      (*(currentBranch[currentBranchLevel]->program))(currentBranch[currentBranchLevel]->locals, currentBranch[currentBranchLevel]->out);
      branchesUpdated[currentBranchLevel--] = 0;
    }
    else
    {
      currentBranch[currentBranchLevel + 1] = currentBranch[currentBranchLevel]->children[branchesUpdated[currentBranchLevel]];
      ++branchesUpdated[currentBranchLevel];
      ++currentBranchLevel;
    }
  }
}

