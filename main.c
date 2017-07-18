#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "engine/window.h"


int shouldClose;


void LoadData(void);
void LoadData(void)
{
}

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  float delta;
  clock_t oldTime = clock();
  while(!shouldClose)
  {
    clock_t time;

    SyncThreads();

    time = clock();
    delta = ((float)(time - oldTime) / (float)CLOCKS_PER_SEC);
    oldTime = time;

    delta = delta;
  }
  return 0;
}


int main(void)
{
    pthread_t workThreadId;

    InitWindow();

    LoadData();

    shouldClose = 0;
    pthread_create(&workThreadId, 0, WorkThread, 0);

    
    WindowMainLoop();
    
    shouldClose = 1;
    
    pthread_join(workThreadId, 0);

    exit(EXIT_SUCCESS);
}
