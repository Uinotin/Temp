#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "engine/window.h"


struct Window window;

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  {
    const TempFloat triangle[] =
      {
	0.0f, 0.5f,
	
      }
  }
  {
    float delta;
    clock_t oldTime = clock();
    while(!ShouldClose(&window))
    {
      clock_t time;

      SyncThreads();

      time = clock();
      delta = ((float)(time - oldTime) / (float)CLOCKS_PER_SEC);
      oldTime = time;

      delta = delta;
    }
  }
  return 0;
}

int main(void)
{
    pthread_t workThreadId;

    InitWindow(&window);

    InitRend(&(window.rend), 32);
    
    pthread_create(&workThreadId, 0, WorkThread, 0);

    
    WindowMainLoop(&window);
    
    
    pthread_join(workThreadId, 0);

    exit(EXIT_SUCCESS);
}
