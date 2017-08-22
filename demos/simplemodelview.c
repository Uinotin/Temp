#include <stdio.h>
#include <string.h>
#include "simplemodelview.h"

const TempFloat verts[] = { 0.0f, 1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f,
                            1.0f, -1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f,
                            -1.0f, -1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f};
const TempInt indices[] = { 0, 1, 2 };

TempChar vShader[] = "#version 420\nlayout(location = 0) in vec4 pos;void main(void){gl_Position = pos;}\n";
TempChar fShader[] = "#version 420\nout vec4 oColor;void main(void){oColor = vec4(1.0, 0.0, 1.0, 1.0);}\n";

void AddSimpleModelView(Scene *scene)
{
  scene->modulePointersSize += sizeof(SimpleModelView);

  if(!(scene->sceneType & TEMP_GRAPHICS_BIT))
  {
    scene->sceneType |= TEMP_GRAPHICS_BIT;
    scene->Init[scene->nModules++] = &InitSimpleModelView;
    scene->Init[scene->nModules++] = &InitBuffers;
    scene->modulePointersSize += sizeof(Buffers);

    scene->Upload[scene->nDrawables] = &UploadBuffers;
    scene->Draw[scene->nDrawables++] = &DrawBuffers;
    scene->Upload[scene->nDrawables] = &UploadSimpleModelView;
    scene->Draw[scene->nDrawables++] = &DrawSimpleModelView;
  }
  else
  {
    int i, nModules = scene->nModules;
    scene->Upload[scene->nDrawables] = &UploadSimpleModelView;
    scene->Draw[scene->nDrawables++] = &DrawSimpleModelView;
    for(i = 0; i < nModules; ++i)
    {
      if(scene->Init[i] == &InitBuffers)
      {
	scene->Init[i] = &InitSimpleModelView;
	scene->Init[scene->nModules++] = &InitBuffers;
	break;
      }
    }
  }
}

void InitSimpleModelView(Scene *scene, TempInt currentIndex)
{
  int i;
  for(i = 0; i < scene->nDrawables; ++i)
    if(scene->Draw[i] == &DrawSimpleModelView)
    {
      scene->drawDataPointers[i] = scene->modulePointers[currentIndex];
      break;
    }
  scene->Alloc[currentIndex] = &AllocSimpleModelView;
  scene->Update[currentIndex] = &UpdateSimpleModelView;
  scene->SetVals[currentIndex] = &SetSimpleModelViewVals;
  if(currentIndex + 1 < scene->nModules)
    scene->modulePointers[currentIndex + 1] = scene->modulePointers[currentIndex] + sizeof(SimpleModelView);
}

TempSizei AllocSimpleModelView(Scene *scene, TempInt currentIndex)
{
  int i, nModules = scene->nModules;
  for(i = currentIndex; i < nModules; ++i)
  {
    if(scene->Init[i] == &InitBuffers)
    { 
      Buffers *buffers = (Buffers *)scene->modulePointers[i];
      AllocBuffer(buffers, TEMP_ARRAY_BUFFER, TEMP_STATIC_DRAW, sizeof(verts)); 
      AllocBuffer(buffers, TEMP_ELEMENT_ARRAY_BUFFER, TEMP_STATIC_DRAW, sizeof(indices));
      break;
    }
  }
  return 0;
}

void SetSimpleModelViewVals(Scene *scene, TempInt currentIndex)
{
  int i, nModules = scene->nModules;
  
  for(i = currentIndex; i < nModules; ++i)
  {
    if(scene->Init[i] == &InitBuffers)
    { 
      SimpleModelView *simpleModelView;
      Buffers *buffers = (Buffers *)scene->modulePointers[i];
      simpleModelView = (SimpleModelView *)scene->modulePointers[currentIndex];
      simpleModelView->handles = buffers->handle;
      simpleModelView->types = buffers->type;
      memcpy(buffers->data[0], verts, sizeof(verts));
      memcpy(buffers->data[1], indices, sizeof(indices));
    }
  }
}

static GLuint CompileShader(const GLchar **shaderSource, GLenum shaderType, GLint len)
{
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, shaderSource, &len);
  glCompileShader(shader);
  {
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
      GLchar log[1024];
      GLint blen = 0;	
      GLsizei slen = 0;

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);       

      if (blen > 1)
      {
        glGetShaderInfoLog(shader, 1024, &slen, log);
        printf("Failed to compile shader:\n%s\n", log);
      }	
      glDeleteShader(shader);
      shader = 0;
    }
  }
  return shader;
}

void UploadSimpleModelView(char *passTypePointer)
{
  TempUInt VAO;
  TempUInt vHandle, fHandle, handle;
  TempChar *vShaderPointer, *fShaderPointer;
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(simpleModelView->types[0], simpleModelView->handles[0]);
  glBindBuffer(simpleModelView->types[1], simpleModelView->handles[1]);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6*sizeof(TempFloat), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6*sizeof(TempFloat), (char *)0 + 4 * sizeof(TempFloat));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);

  handle = glCreateProgram();
  vShaderPointer = vShader;
  vHandle = CompileShader((const GLchar **)&vShaderPointer, GL_VERTEX_SHADER, sizeof(vShader));
  fShaderPointer = fShader;
  fHandle = CompileShader((const GLchar **)&fShaderPointer, GL_FRAGMENT_SHADER, sizeof(fShader));
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  glLinkProgram(handle);
  simpleModelView->program = handle;
  simpleModelView->VAO = VAO;
}

void UpdateSimpleModelView(char *passTypePointer, float deltaTime)
{

}

void DrawSimpleModelView(char *passTypePointer)
{
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  glBindVertexArray(simpleModelView->VAO);
  glUseProgram(simpleModelView->program);

  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glBindVertexArray(0);
}
