#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glstate.h"

void InitRenderer(void)
{
  glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uboOffsetAlignment);
  assert(uboOffsetAlignment <= 1024);
  
}

int GetMaxTextures(void)
{
  int nMaxTextures;

  glGetIntegerv(GL_MAX_TEXTURE_UNITS, &nMaxTextures);
  return nMaxTextures;
}
