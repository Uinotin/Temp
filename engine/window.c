#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "window.h"

pthread_barrier_t barrier;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


void InitWindow(Window *window)
{
  GLFWwindow* glfwWindow;
  //currentScene = 0;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#if OPENGL_DEBUG_OUTPUT
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindow = glfwCreateWindow(1280, 720, "Temp", NULL, NULL);
    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
 
    glfwSwapInterval(1);

    glfwSetKeyCallback(glfwWindow, key_callback);

    window->systemWindow = (char *)glfwWindow;

    InitRend(&(window->rend));


}

int ShouldClose(Window *window)
{
  return glfwWindowShouldClose((GLFWwindow *)(window->systemWindow));
}

void WindowMainLoop(Window *window)
{
  GLFWwindow *glfwWindow = (GLFWwindow *)window->systemWindow;
  while (!glfwWindowShouldClose(glfwWindow))
  {
    ExecCommands(&(window->rend));

    glfwSwapBuffers(glfwWindow);
    
    glfwPollEvents();

    SyncThreads();
    SyncThreads();
  }

  DestroyRend(&(window->rend));

  glfwDestroyWindow(glfwWindow);

  glfwTerminate();
}

