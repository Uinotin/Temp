#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "modelviewmodule.h"

static GLuint CompileShader(const char *filename, GLenum shaderType)
{ 
  FILE *file;
  GLchar fileString[4096];
  GLuint shader;
  int stringIndex;
  const char *pathName = "shader/\0";
  strcpy(fileString, pathName); 
  strcpy(fileString + strlen(pathName), filename);
  stringIndex = 0;
  shader = 0;
  
  file = fopen(fileString, "r");
  if (file==NULL)
    printf("Could not open shader file %s\n", fileString);
  else
  {
    do {
      assert(stringIndex < stringSize);
      fileString[stringIndex] = fgetc (file);
    } while (fileString[stringIndex++] != EOF);
    fileString[--stringIndex] = 0;
    fclose (file);
  }
  
  if (stringIndex)
  {
    GLchar *fileStringPointer = fileString;
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, (const GLchar **)&fileStringPointer, NULL);
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
  }
  
  return shader;
}

static TempUInt LoadTGA(const char *fileName, int isSRGB)
{
	unsigned char length = 0;
	unsigned char imgType = 0;

	unsigned short width = 0;
	unsigned short height = 0;
	unsigned char bits = 0;

	int channels = 0;
	int stride = 0;

	unsigned char *data;


	GLuint texture = 0;

	FILE *file = fopen(fileName, "rb");
	fseek(file, 0, SEEK_SET);
	fread(&length, sizeof(unsigned char), 1, file);
	fseek(file, 1, SEEK_CUR);
	fread(&imgType, sizeof(unsigned char), 1, file);
	fseek(file, 9, SEEK_CUR);
	fread(&width, sizeof(unsigned short), 1, file);
	fread(&height, sizeof(unsigned short), 1, file);
	fread(&bits, sizeof(unsigned char), 1, file);
	fseek(file, length + 1, SEEK_CUR);

	printf("Filename %s length %u type %u width %u height %u bits %u\n", fileName, length, imgType, width, height, bits);

	channels = (int)(bits / 8);
	stride = channels * width;
	data = (unsigned char *)malloc(sizeof(unsigned char)*stride*height);
	int y;
	for(y = height - 1; y >= 0; y--)
	{
		unsigned char *line = &(data[stride*y]);
		fread(line, stride, 1, file);

		int i;
		for(i = 0; i < stride; i += channels)
		{
			int temp = line[i];
			line[i] = line[i + 2];
			line[i + 2] = temp;
		}
	}
	fclose(file);

	//glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(channels == 4)
	  glTexImage2D(GL_TEXTURE_2D, 0, isSRGB ? GL_SRGB8_ALPHA8 : GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	else glTexImage2D(GL_TEXTURE_2D, 0, isSRGB ? GL_SRGB8 : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	free(data);

	return texture;
}

static void Perspective(TempFloat *mat, TempFloat fov, TempFloat near, TempFloat far, TempFloat aspect)
{
  mat[0] = (float)atan((double)(fov/2.0f))/aspect;
  mat[5] = (float)atan((double)(fov/2.0f));
  mat[10] = -(far+near)/(far-near);
  mat[11] = -(2.0f*far*near)/(far-near);
  mat[14] = -1.0f;
  mat[15] = 0;
}


static void Identity(TempFloat *mat)
{
  mat[0] = 1;
  mat[1] = 0;
  mat[2] = 0;
  mat[3] = 0;

  mat[4] = 0;
  mat[5] = 1;
  mat[6] = 0;
  mat[7] = 0;

  mat[8] = 0;
  mat[9] = 0;
  mat[10] = 1;
  mat[11] = 0;

  mat[12] = 0;
  mat[13] = 0;
  mat[14] = 0;
  mat[15] = 1;
}

static void Identity3v(TempFloat *mat)
{
  mat[0] = 1;
  mat[1] = 0;
  mat[2] = 0;
  
  mat[3] = 0;
  mat[4] = 1;
  mat[5] = 0;
  
  mat[6] = 0;
  mat[7] = 0;
  mat[8] = 1;
}

void LoadDrawModelView(ProgramTreeNode *drawNode)
{
  TempUInt handle, vHandle, fHandle;
  DrawModelViewData *locals;
  drawNode->children = 0;
  drawNode->locals = (void *)malloc(sizeof(DrawModelViewData));
  drawNode->nChildren = 0;
  drawNode->program = *DrawModelView;
  drawNode->freeProgram = *FreeDrawModelView;

  locals = (DrawModelViewData *)drawNode->locals;

  handle = glCreateProgram();
  vHandle = CompileShader("character.vs", GL_VERTEX_SHADER);
  fHandle = CompileShader("character.fs", GL_FRAGMENT_SHADER);
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  
  glLinkProgram(handle);
  locals->texLoc = glGetUniformLocation(handle, "texSampler");
  locals->normLoc = glGetUniformLocation(handle, "normSampler");
  locals->projMatLoc = glGetUniformLocation(handle, "projMat");
  locals->viewMatLoc = glGetUniformLocation(handle, "viewMat");
  locals->worldMatLoc = glGetUniformLocation(handle, "worldMat");
  locals->normalMatLoc = glGetUniformLocation(handle, "normalMat");
  locals->lightPosLoc = glGetUniformLocation(handle, "lightPos");
  locals->shaderHandle = handle;
  
  {
    FILE *file;
    char *data;
    int nVerts, nIndices;
    file = fopen("testchar", "rb");
    fread(&nVerts, sizeof(int), 1, file);
    fread(&nIndices, sizeof(int), 1, file);
    data = (char *)malloc(nVerts * 14 * sizeof(float) + nIndices * sizeof(unsigned int));
    fread(data, sizeof(float), (unsigned int)nVerts * 8, file);
    fread(data + nVerts * 14 * sizeof(float), sizeof(unsigned int), (unsigned int)nIndices, file);
    fclose(file);
    locals->vertCount = nVerts;
    locals->indxCount = nIndices;

    {
      TempUInt VAO, VBO, IBO;
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, nVerts * 14 * sizeof(float), data, GL_STATIC_DRAW);
      glGenBuffers(1, &IBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), data + nVerts * 14 * sizeof(float), GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), 0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 3 * sizeof(TempFloat));
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 5 * sizeof(TempFloat));
      //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + nVerts * 8 * sizeof(TempFloat));
      //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + nVerts * 11 * sizeof(TempFloat));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      //glEnableVertexAttribArray(3);
      //glEnableVertexAttribArray(4);
      glBindVertexArray(0);

      locals->VAO = VAO;
      locals->VBO = VBO;
      locals->IBO = IBO;
    }
    free(data);
  }

  locals->texture = LoadTGA("texture_001.tga", 1);
  locals->normalMap = LoadTGA("normals_05.tga", 0);
  Perspective(locals->projMat, 6.1, 0.1f, 10.0f, 1280.0f/720.0f);
  Identity(locals->projMat);
  Identity(locals->viewMat);
  Identity(locals->worldMat);
  Identity3v(locals->normalMat);

  glClearColor(0.129f, 0.129f, 0.129f, 1.0f);
}

void DrawModelView(ProgramTreeNode *drawProgram)
{
  const TempFloat *projMatPtr, *worldMatPtr, *viewMatPtr, *normalMatPtr;
  DrawModelViewData *drawData = (DrawModelViewData *)(drawProgram->locals);
  projMatPtr = drawData->projMat;
  viewMatPtr = drawData->viewMat;
  worldMatPtr = drawData->worldMat;
  normalMatPtr = drawData->normalMat;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //BindFramebuffer(drawData->inoutFbo);
  //EnableFramebufferSRGB(drawData->inoutFbo);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glBindVertexArray(drawData->VAO);
  glUseProgram(drawData->shaderHandle);
  glUniform1i(drawData->texLoc, 0);
  glUniform1i(drawData->normLoc, 1);
  glUniformMatrix4fv(drawData->worldMatLoc, GL_TRUE, 1, worldMatPtr);
  glUniformMatrix4fv(drawData->projMatLoc, GL_TRUE, 1, projMatPtr);
  glUniformMatrix4fv(drawData->viewMatLoc, GL_TRUE, 1, viewMatPtr);
  glUniformMatrix3fv(drawData->normalMatLoc, GL_TRUE, 1, normalMatPtr);
  glUniform3fv(drawData->lightPosLoc, 1, drawData->lightPosition);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, drawData->texture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, drawData->normalMap);
  

  glDrawElements(GL_TRIANGLES, drawData->indxCount, GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glBindVertexArray(0);
}

void FreeDrawModelView(ProgramTreeNode *programTreeNode)
{
  
}
