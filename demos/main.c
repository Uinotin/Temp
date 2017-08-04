#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "../engine/window.h"
#include "../engine/buffer.h"
#include "../engine/commands.h"
#include "../engine/vertexarray.h"
#include "../engine/program.h"

Window window;
Rend *rend;

void *WorkThread(void *arg);
void *WorkThread(void *arg)
{
  CommandQueue commandQueue;
  QueueData *queueData = &(commandQueue.queueData);
  BufferPtr bufferPtr;
  VertexArrayPtr VAOPtr;
  ProgramPtr programPtr;
  {
    const TempFloat triangle[] =
      {
	0.0f, 0.5f,
	0.5f, 0.0f,
	-0.5f, 0.0f
      };

    TempChar vShader[] = "#version 420\nlayout(location = 0) in vec2 pos;void main(void){gl_Position = vec4(pos.x, pos.y, pos.x, 1.0);}\n";
    TempChar fShader[] = "#version 420\nout vec4 oColor;void main(void){oColor = vec4(1.0, 0.0, 1.0, 1.0);}\n";

    InitCommandQueue(&commandQueue, 7, sizeof(vShader) + sizeof(fShader) + 255);
    
    
    GenBuffers(&bufferPtr, 1);
    SetBufferData(bufferPtr.buffers, sizeof(triangle), (char *)triangle);
    bufferPtr.buffers->handleData.type = TEMP_ARRAY_BUFFER;
    GenVAOs(queueData, 1, &VAOPtr);
    BindVAOPtr(queueData, &VAOPtr);
    UploadBuffers(queueData, &bufferPtr);

    {
      VertexAttrib attrib = {.index = 0, .stride = sizeof(TempFloat) * 2, {.size = 2, .type = TEMP_FLOAT, .offset = 0 }};
      AttribPointers(queueData, 1, &attrib);
    }
    BindVAO(queueData, 0);
    GenPrograms(queueData, 1, &programPtr);
    LoadProgram(queueData, &programPtr, sizeof(vShader), sizeof(fShader), vShader, fShader);

    StartAppend(&(rend->commandQueueList));
    SyncThreads();
    Append(&(rend->commandQueueList), &commandQueue);
    FinishAppend(&(rend->commandQueueList));
    
    SyncThreads();
    SyncThreads();

    EmptyCommandQueue(&commandQueue);
    Clear(queueData, TEMP_COLOR_BUFFER_BIT);
    BindVAO(queueData, *(VAOPtr.vaoHandle));
    UseProgram(queueData, *(programPtr.program));
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
