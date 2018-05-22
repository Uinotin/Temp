#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "../engine/window.h"
#include "modelviewmodule.h"


Window window;

int main(void)
{

    InitWindow(&window);

    StartProgramTree(&(window.programTree), &LoadDrawModelView);

    WindowMainLoop(&window);

    exit(EXIT_SUCCESS);
}
