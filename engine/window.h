#ifndef WINDOW_H
#define WINDOW_H

#include "programtree.h"

typedef struct 
{
  ProgramTree programTree;
  char *systemWindow;
} Window;

void InitWindow(Window *window, const char *name);
int ShouldClose(Window *window);
void WindowMainLoop(Window *window);

#endif
