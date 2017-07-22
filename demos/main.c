#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "../engine/window.h"


struct Window window;

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  struct CommandQueue commandQueue;
  struct QueueData *queueData = queueData;
  {
    const TempFloat triangle[] =
      {
	0.0f, 0.5f,
	0.5f, 0.0f,
	-0.5f, 0.0f
      };

    const TempChar vShader[] = "#version 420\nlayout(location = 0) in vec2 pos;void main(void){gl_Position = vec4(pos.x, pos.y, -0.1, 1.0);}";
    const TempChar fShader[] = "#version 420\nout vec4 oColor;void main(void){oColor = vec4(1.0, 0.0, 1.0, 1.0);}";
    const size_t indicesLensAndOffsets[] = {0, 0, 2};

    AllocHandles(&(window.rend), 4);
    StartCommandQueue(&commandQueue, 7,
                      6 * sizeof(TempUInt) +
		      sizeof(TempEnum) +
		      4 * sizeof(size_t) +
		      sizeof(triangle) +
		      sizeof(vShader) +
		      sizeof(fShader) +
		      sizeof(indicesLensAndOffsets));
    
    GenVAO(queueData, 1);
    BindVAO(queueData, 1);
    UploadBuffer(queueData, TEMP_ARRAY_BUFFER, 2, sizeof(triangle), triangle);
    AttribPointers(queueData, sizeof(TempFloat) * 2, 1, indicesLensAndOffsets);
    BindVAO(queueData, 0);
    LoadProgram(queueData, 3, sizeof(vShader), sizeof(fShader), vShader, fShader);

    SyncThreads();
    StartAppend(&(window.rend));
    SyncThreads();
    Append(&(window.rend), &commandQueue);
    FinishAppend(&(window.rend));
    
    SyncThreads();

    EmptyCommandQueue(&commandQueue);
    BindVAO(queueData, 1);
    UseProgram(queueData, 3);
    Draw(queueData, TEMP_TRIANGLES, 0, 3);
    UseProgram(queueData, 0);
    BindVAO(queueData, 0);

    SyncThreads();
  }
  {
    float delta;
    clock_t oldTime = clock();
    while(!ShouldClose(&window))
    {
      clock_t time;

      SyncThreads();
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

    InitRend(&(window.rend));
    
    pthread_create(&workThreadId, 0, WorkThread, 0);

    
    WindowMainLoop(&window);
    
    
    pthread_join(workThreadId, 0);

    exit(EXIT_SUCCESS);
}
