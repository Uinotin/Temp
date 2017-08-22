#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "../engine/window.h"
#include "simplemodelview.h"


Window window;
Scene scene;

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  {
    float delta;
    clock_t oldTime = clock();
    while(!ShouldClose(&window))
    {
      clock_t time;


      time = clock();
      delta = ((float)(time - oldTime) / (float)CLOCKS_PER_SEC);
      oldTime = time;

      delta = delta;

      UpdateScene(&scene, delta);
      
      SyncThreads();
      SyncThreads();
    }
  }
  return 0;
}

int main(void)
{
    pthread_t workThreadId;

    InitWindow(&window);
    InitScene(&scene);
    AddSimpleModelView(&scene);
    LoadScene(&scene);
    
    SetCurrentScene(&window, &scene);

    pthread_create(&workThreadId, 0, WorkThread, 0);

    WindowMainLoop(&window);
    
    pthread_join(workThreadId, 0);

    exit(EXIT_SUCCESS);
}
