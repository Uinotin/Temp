#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

#include "window.h"

#if OPENGL_DEBUG_OUTPUT
static void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
  {
    printf("GL_DEBUG_SEVERITY_");
    switch (severity)
    {
      case GL_DEBUG_SEVERITY_HIGH:
	printf("HIGH");
	break;
      case GL_DEBUG_SEVERITY_MEDIUM:
	printf("MEDIUM");
	break;
      case GL_DEBUG_SEVERITY_LOW:
	printf("LOW");
	break;
      case GL_DEBUG_SEVERITY_NOTIFICATION:
	printf("NOTIFICATION");
	break;
    }
    printf(":\n%s\n", message);
  }
}
#endif

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


void InitWindow(Window *window, const char *name)
{
  GLFWwindow* glfwWindow;
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 8);
#if OPENGL_DEBUG_OUTPUT
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindow = glfwCreateWindow(1280, 720, name, NULL, NULL);
  if (!glfwWindow)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(glfwWindow);
 
  glfwSwapInterval(1);

  glfwSetKeyCallback(glfwWindow, key_callback);

  window->systemWindow = (char *)glfwWindow;

  {
    GLenum err;
    glewExperimental = GL_TRUE;
    err = glewInit();
    if (GLEW_OK != err)
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); 
    }
  }

#if OPENGL_DEBUG_OUTPUT
    if(glDebugMessageCallback)
    {
      GLuint unusedIds = 0;
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback((GLDEBUGPROC)openglCallbackFunction, 0);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
			    0,
			    &unusedIds,
			    1);
    }
#endif
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
    UpdateProgramTree(&(window->programTree));
    
    glfwSwapBuffers(glfwWindow);
    
    glfwPollEvents();
  }

  glfwDestroyWindow(glfwWindow);

  glfwTerminate();
}
