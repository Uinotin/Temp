#ifndef WINDOW_H
#define WINDOW_H

#include "rend.h"

struct Window
{
  struct Rend rend;
  char *systemWindow;
};

void InitWindow(struct Window *window);
int ShouldClose(struct Window *window);
void WindowMainLoop(struct Window *window);
void SyncThreads(void);

#endif
