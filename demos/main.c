#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "../engine/window.h"
#include "../engine/buffer.h"
#include "../engine/commands.h"

Window window;
Rend *rend;

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  CommandQueue commandQueue;
  QueueData *queueData = &(commandQueue.queueData);
  Buffer buffer;
  TempUInt VAO, program;
  {
    const TempFloat triangle[] =
      {
	0.0f, 0.5f,
	0.5f, 0.0f,
	-0.5f, 0.0f
      };

    TempChar vShader[] = "#version 420\nlayout(location = 0) in vec2 pos;void main(void){gl_Position = vec4(pos.x, pos.y, pos.x, 1.0);}\n";
    TempChar fShader[] = "#version 420\nout vec4 oColor;void main(void){oColor = vec4(1.0, 0.0, 1.0, 1.0);}\n";
    TempUInt indicesLensAndOffsets[] = {0, 2, 0};

    AllocHandles(rend, 4, 1, 1);
    StartCommandQueue(&commandQueue, 7,
                      6 * sizeof(TempUInt) +
		      sizeof(TempEnum) +
		      4 * sizeof(size_t) +
		      sizeof(triangle) +
		      sizeof(vShader) +
		      sizeof(fShader) +
		      sizeof(indicesLensAndOffsets));

    CreateBuffer(rend, &buffer, TEMP_ARRAY_BUFFER, sizeof(triangle));
    buffer.data = (char*)triangle;

    VAO = GetVAOHandle(rend);
    BindVAO(queueData, VAO);
    UploadBuffer(queueData, &buffer);
    AttribPointers(queueData, sizeof(TempFloat) * 2, 1, indicesLensAndOffsets);
    BindVAO(queueData, 0);
    program = GetProgramHandle(rend);
    LoadProgram(queueData, program, sizeof(vShader), sizeof(fShader), vShader, fShader);

    SyncThreads();
    StartAppend(rend);
    SyncThreads();
    Append(rend, &commandQueue);
    FinishAppend(rend);
    
    SyncThreads();

    EmptyCommandQueue(&commandQueue);
    BindVAO(queueData, VAO);
    UseProgram(queueData, program);
    DrawArrays(queueData, TEMP_TRIANGLES, 0, 3);
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

    rend = &(window.rend);

    
    pthread_create(&workThreadId, 0, WorkThread, 0);

    WindowMainLoop(&window);
    
    pthread_join(workThreadId, 0);

    exit(EXIT_SUCCESS);
}
