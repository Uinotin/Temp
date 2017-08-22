#ifndef WINDOW_H
#define WINDOW_H

#include "scene.h"

typedef struct 
{
  Scene *currentScene;
  char *systemWindow;
} Window;

void InitWindow(Window *window);
int ShouldClose(Window *window);
void WindowMainLoop(Window *window);
void SetCurrentScene(Window *window, Scene *scene);
void SyncThreads(void);

#endif
