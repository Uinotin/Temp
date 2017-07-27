#ifndef WINDOW_H
#define WINDOW_H

#include "rend.h"

typedef struct 
{
  Rend rend;
  char *systemWindow;
} Window;

void InitWindow(Window *window);
int ShouldClose(Window *window);
void WindowMainLoop(Window *window);

#endif
