#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "simplemodelview.h"

const TempFloat verts[] = { 0.0f, 1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f,
                            1.0f, -1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f,
                            -1.0f, -1.0f, 0.0f, 1.0f,
                            0.4f, 0.3f};
const TempInt indices[] = { 0, 1, 2 };

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
    scene->UpdateDrawData[scene->nDrawables] = &UpdateBuffersDrawData;
    scene->Draw[scene->nDrawables++] = &DrawBuffers;
    scene->Upload[scene->nDrawables] = &UploadSimpleModelView;
    scene->UpdateDrawData[scene->nDrawables] = &UpdateSimpleModelViewDrawData;
    scene->Draw[scene->nDrawables++] = &DrawSimpleModelView;
  }
  else
  {
    int i, nModules = scene->nModules;
    scene->Upload[scene->nDrawables] = &UploadSimpleModelView;
    scene->UpdateDrawData[scene->nDrawables] = &UpdateSimpleModelViewDrawData;
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
  scene->Key[currentIndex] = &SimpleModelViewKey;
  if(currentIndex + 1 < scene->nModules)
    scene->modulePointers[currentIndex + 1] = scene->modulePointers[currentIndex] + sizeof(SimpleModelView);
}

TempSizei AllocSimpleModelView(Scene *scene, TempInt currentIndex)
{
  FILE *file;
  int nVerts, nIndices;
  int i, nModules = scene->nModules;
  file = fopen("characterbin", "rb");
  fread(&nVerts, sizeof(int), 1, file);
  fread(&nIndices, sizeof(int), 1, file);
  fclose(file);
  
  for(i = currentIndex; i < nModules; ++i)
  {
    if(scene->Init[i] == &InitBuffers)
    { 
      Buffers *buffers = (Buffers *)scene->modulePointers[i];
      AllocBuffer(buffers, TEMP_ARRAY_BUFFER, TEMP_STATIC_DRAW, (unsigned int)nVerts * 14 * sizeof(float)); 
      AllocBuffer(buffers, TEMP_ELEMENT_ARRAY_BUFFER, TEMP_STATIC_DRAW, (unsigned int)nIndices * sizeof(unsigned int));
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
      int nVerts, nIndices;
      FILE *file;
      Buffers *buffers = (Buffers *)scene->modulePointers[i];
      simpleModelView = (SimpleModelView *)scene->modulePointers[currentIndex];
      simpleModelView->handles = buffers->handle;
      simpleModelView->types = buffers->type;
      
      file = fopen("testchar", "rb");
      fread(&nVerts, sizeof(int), 1, file);
      fread(&nIndices, sizeof(int), 1, file);
      fread(buffers->data[0], sizeof(float), (unsigned int)nVerts * 8, file);
      fread(buffers->data[1], sizeof(unsigned int), (unsigned int)nIndices, file);
      fclose(file);
      simpleModelView->vertCount = nVerts;
      simpleModelView->indxCount = nIndices;
      simpleModelView->upKeyDown = 0;
      simpleModelView->downKeyDown = 0;
      simpleModelView->camera.rotAlongX = 0;
      simpleModelView->camera.rotAlongY = 0;
      simpleModelView->camera.location[0] = 0;
      simpleModelView->camera.location[1] = 0.5f;
      simpleModelView->camera.location[2] = 1.0f;
      simpleModelView->light.position[0] = 1.0f;
      simpleModelView->light.position[1] = 1.0f;
      simpleModelView->light.position[2] = -1.0f;

      {
	int i;
	TempUInt *__restrict__ indices;
	TempFloat *verts, *bitangents, *tangents = (TempFloat *)buffers->data[0] + nVerts * 8;
	verts = (TempFloat *)buffers->data[0];
	bitangents = (TempFloat *)buffers->data[0] + nVerts * 11;
	indices = (TempUInt *)buffers->data[1];
	for(i = 0; i < 3 *nVerts; ++i)
	{
	  bitangents[i] = 0.0f;
	  tangents[i] = 0.0f;
	}

	for(i = 0; i < nIndices/3; ++i)
	{
	  TempFloat r, *v0, *v1, *v2, *uv0, *uv1, *uv2;
	  TempFloat deltaUV1[2], deltaUV2[2], deltaPos1[3], deltaPos2[3];
	  v0 = verts + indices[0] * 8;
	  uv0 =verts + indices[0] * 8 + 3;
	  v1 = verts + indices[1] * 8;
	  uv1 =verts + indices[1] * 8 + 3;
	  v2 = verts + indices[2] * 8;
	  uv2 =verts + indices[2] * 8 + 3;

	  deltaPos1[0] = v1[0] - v0[0];
	  deltaPos1[1] = v1[1] - v0[1];
	  deltaPos1[2] = v1[2] - v0[2];
	 
	  deltaPos2[0] = v2[0] - v0[0];
	  deltaPos2[1] = v2[1] - v0[1];
	  deltaPos2[2] = v2[2] - v0[2];

	  deltaUV1[0] = uv1[0] - uv0[0];
	  deltaUV1[1] = uv1[1] - uv0[1];

	  deltaUV2[0] = uv2[0] - uv0[0];
	  deltaUV2[1] = uv2[1] - uv0[1];

	  r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);

	  tangents[indices[0] * 3] += (deltaPos1[0] * deltaUV2[1] - deltaPos2[0] * deltaUV1[1]) * r;
	  tangents[indices[0] * 3 + 1] += (deltaPos1[1] * deltaUV2[1] - deltaPos2[1] * deltaUV1[1]) * r;
	  tangents[indices[0] * 3 + 2] += (deltaPos1[2] * deltaUV2[1] - deltaPos2[2] * deltaUV1[1]) * r;

	  bitangents[indices[0] * 3 + 0] += (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0]) * r;
	  bitangents[indices[0] * 3 + 1] += (deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0]) * r;
	  bitangents[indices[0] * 3 + 2] += (deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0]) * r;

	  tangents[indices[1] * 3] += (deltaPos1[0] * deltaUV2[1] - deltaPos2[0] * deltaUV1[1]) * r;
	  tangents[indices[1] * 3 + 1] += (deltaPos1[1] * deltaUV2[1] - deltaPos2[1] * deltaUV1[1]) * r;
	  tangents[indices[1] * 3 + 2] += (deltaPos1[2] * deltaUV2[1] - deltaPos2[2] * deltaUV1[1]) * r;
	  
	  bitangents[indices[1] * 3 + 0] += (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0]) * r;
	  bitangents[indices[1] * 3 + 1] += (deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0]) * r;
	  bitangents[indices[1] * 3 + 2] += (deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0]) * r;

	  tangents[indices[2] * 3] += (deltaPos1[0] * deltaUV2[1] - deltaPos2[0] * deltaUV1[1]) * r;
	  tangents[indices[2] * 3 + 1] += (deltaPos1[1] * deltaUV2[1] - deltaPos2[1] * deltaUV1[1]) * r;
	  tangents[indices[2] * 3 + 2] += (deltaPos1[2] * deltaUV2[1] - deltaPos2[2] * deltaUV1[1]) * r;
	  
	  bitangents[indices[2] * 3 + 0] += (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0]) * r;
	  bitangents[indices[2] * 3 + 1] += (deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0]) * r;
	  bitangents[indices[2] * 3 + 2] += (deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0]) * r;

	  indices += 3;
	}

	for(i = 0; i < nVerts; ++i)
        {
	  float len = (float)sqrt(
			     (double)(tangents[i * 3 + 0]*tangents[i * 3 + 0] +
	                              tangents[i * 3 + 1]*tangents[i * 3 + 1] +
				      tangents[i * 3 + 2]*tangents[i * 3 + 2]));
	  tangents[i * 3 + 0] /= len;
	  tangents[i * 3 + 1] /= len;
	  tangents[i * 3 + 2] /= len;

	  len = (float)sqrt(
			     (double)(bitangents[i * 3 + 0]*bitangents[i * 3 + 0] +
	                              bitangents[i * 3 + 1]*bitangents[i * 3 + 1] +
				      bitangents[i * 3 + 2]*bitangents[i * 3 + 2]));
	  bitangents[i * 3 + 0] /= len;
	  bitangents[i * 3 + 1] /= len;
	  bitangents[i * 3 + 2] /= len;
	}
      }
    }
  }
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

/*
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
*/

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

void UploadSimpleModelView(char *passTypePointer)
{
  TempUInt VAO;
  TempUInt vHandle, fHandle, handle;
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(simpleModelView->types[0], simpleModelView->handles[0]);
  glBindBuffer(simpleModelView->types[1], simpleModelView->handles[1]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 3 * sizeof(TempFloat));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 5 * sizeof(TempFloat));
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + simpleModelView->vertCount* 8 * sizeof(TempFloat));
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + simpleModelView->vertCount * 11 * sizeof(TempFloat));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glBindVertexArray(0);

  handle = glCreateProgram();
  vHandle = CompileShader("character.vs", GL_VERTEX_SHADER);
  fHandle = CompileShader("character.fs", GL_FRAGMENT_SHADER);
  glAttachShader(handle, vHandle);
  glAttachShader(handle, fHandle);
  
  glLinkProgram(handle);
  simpleModelView->texUniformHandle = glGetUniformLocation(handle, "texSampler");
  simpleModelView->normalUniformHandle = glGetUniformLocation(handle, "normSampler");
  simpleModelView->camera.projMatLocation = glGetUniformLocation(handle, "projMat");
  simpleModelView->camera.viewMatLocation = glGetUniformLocation(handle, "viewMat");
  simpleModelView->worldMatLocation = glGetUniformLocation(handle, "worldMat");
  simpleModelView->normalMatLocation = glGetUniformLocation(handle, "normalMat");
  simpleModelView->light.lightPosLocation = glGetUniformLocation(handle, "lightPos");
  simpleModelView->program = handle;
  simpleModelView->VAO = VAO;

  simpleModelView->texture = LoadTGA("texture_001.tga", 1);
  simpleModelView->normalMap = LoadTGA("normals_05.tga", 0);

  glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.129f, 0.129f, 0.129f, 1.0f);

  Identity(simpleModelView->camera.projMat);
  Identity(simpleModelView->camera.viewMat);
  Identity(simpleModelView->worldMat);

  Perspective(simpleModelView->camera.projMat, 6.1, 0.1f, 10.0f, 1280.0f/720.0f);
  //Identity(simpleModelView->camera.projMat);

}

static void RotateAlongY(TempFloat *mat, TempFloat angle)
{
  TempFloat tempMat[16], cosAngle, sinAngle;
  cosAngle = (float)cos((double)angle);
  sinAngle = (float)sin((double)angle);

  tempMat[0] = cosAngle * mat[0] + -sinAngle * mat[8];
  tempMat[4] = 1.0f * mat[4];
  tempMat[8] = sinAngle * mat[0] + cosAngle * mat[8];
  tempMat[12] = 1.0f * mat[12];

  tempMat[1] = cosAngle * mat[1] + -sinAngle * mat[9];
  tempMat[5] = 1.0f * mat[5];
  tempMat[9] = sinAngle * mat[1] + cosAngle * mat[9];
  tempMat[13] = 1.0f * mat[13];

  tempMat[2] = cosAngle * mat[2] + -sinAngle * mat[10];
  tempMat[6] = 1.0f * mat[6];
  tempMat[10] = sinAngle * mat[2] + cosAngle * mat[10];
  tempMat[14] = 1.0f * mat[14];

  tempMat[3] = cosAngle * mat[3] + -sinAngle * mat[11];
  tempMat[7] = 1.0f * mat[7];
  tempMat[11] = sinAngle * mat[3] + cosAngle * mat[11];
  tempMat[15] = 1.0f * mat[15];

  memcpy(mat, tempMat, sizeof(TempFloat) *16);
}

static void RotateAlongX(TempFloat *mat, TempFloat angle)
{
  TempFloat tempMat[16];
  TempFloat rotMat[] = {1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f};

  rotMat[5] = (float)cos((double)angle);
  rotMat[6] = -((float)sin((double)angle));
  rotMat[9] = (float)sin((double)angle);
  rotMat[10] = (float)cos((double)angle);

  tempMat[0] = rotMat[0] * mat[0] + rotMat[1] * mat[4] + rotMat[2] * mat[8] + rotMat[3] * mat[12];
  tempMat[4] = rotMat[4] * mat[0] + rotMat[5] * mat[4] + rotMat[6] * mat[8] + rotMat[7] * mat[12];
  tempMat[8] = rotMat[8] * mat[0] + rotMat[9] * mat[4] + rotMat[10] * mat[8] + rotMat[11] * mat[12];
  tempMat[12] = rotMat[12] * mat[0] + rotMat[13] * mat[4] + rotMat[14] * mat[8] + rotMat[15] * mat[12];

  tempMat[1] = rotMat[0] * mat[1] + rotMat[1] * mat[5] + rotMat[2] * mat[9] + rotMat[3] * mat[13];
  tempMat[5] = rotMat[4] * mat[1] + rotMat[5] * mat[5] + rotMat[6] * mat[9] + rotMat[7] * mat[13];
  tempMat[9] = rotMat[8] * mat[1] + rotMat[9] * mat[5] + rotMat[10] * mat[9] + rotMat[11] * mat[13];
  tempMat[13] = rotMat[12] * mat[1] + rotMat[13] * mat[5] + rotMat[14] * mat[9] + rotMat[15] * mat[13];

  tempMat[2] = rotMat[0] * mat[2] + rotMat[1] * mat[6] + rotMat[2] * mat[10] + rotMat[3] * mat[14];
  tempMat[6] = rotMat[4] * mat[2] + rotMat[5] * mat[6] + rotMat[6] * mat[10] + rotMat[7] * mat[14];
  tempMat[10] = rotMat[8] * mat[2] + rotMat[9] * mat[6] + rotMat[10] * mat[10] + rotMat[11] * mat[14];
  tempMat[14] = rotMat[12] * mat[2] + rotMat[13] * mat[6] + rotMat[14] * mat[10] + rotMat[15] * mat[14];

  tempMat[3] = rotMat[0] * mat[3] + rotMat[1] * mat[7] + rotMat[2] * mat[11] + rotMat[3] * mat[15];
  tempMat[7] = rotMat[4] * mat[3] + rotMat[5] * mat[7] + rotMat[6] * mat[11] + rotMat[7] * mat[15];
  tempMat[11] = rotMat[8] * mat[3] + rotMat[9] * mat[7] + rotMat[10] * mat[11] + rotMat[11] * mat[15];
  tempMat[15] = rotMat[12] * mat[3] + rotMat[13] * mat[7] + rotMat[14] * mat[11] + rotMat[15] * mat[15];

  memcpy(mat, tempMat, sizeof(TempFloat) *16);
}
static void RotateAlongX3v(TempFloat *mat, TempFloat angle)
{
  TempFloat tempMat[9];
  TempFloat rotMat[] = {1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f};

  rotMat[4] = (float)cos((double)angle);
  rotMat[5] = -((float)sin((double)angle));
  rotMat[7] = (float)sin((double)angle);
  rotMat[8] = (float)cos((double)angle);
  
  tempMat[0] = rotMat[0] * mat[0] + rotMat[1] * mat[3] + rotMat[2] * mat[6];
  tempMat[3] = rotMat[3] * mat[0] + rotMat[4] * mat[3] + rotMat[5] * mat[6];
  tempMat[6] = rotMat[6] * mat[0] + rotMat[7] * mat[3] + rotMat[8] * mat[6];

  tempMat[1] = rotMat[0] * mat[1] + rotMat[1] * mat[4] + rotMat[2] * mat[7];
  tempMat[4] = rotMat[3] * mat[1] + rotMat[4] * mat[4] + rotMat[5] * mat[7];
  tempMat[7] = rotMat[6] * mat[1] + rotMat[7] * mat[4] + rotMat[8] * mat[7];

  tempMat[2] = rotMat[0] * mat[2] + rotMat[1] * mat[5] + rotMat[2] * mat[8];
  tempMat[5] = rotMat[3] * mat[2] + rotMat[4] * mat[5] + rotMat[5] * mat[8];
  tempMat[8] = rotMat[6] * mat[2] + rotMat[7] * mat[5] + rotMat[8] * mat[8];

  memcpy(mat, tempMat, sizeof(TempFloat) * 9);
}

static void RotateAlongY3v(TempFloat *mat, TempFloat angle)
{
  TempFloat tempMat[9];
  TempFloat rotMat[] = {1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f};

  rotMat[0] = (float)cos((double)angle);
  rotMat[2] = -((float)sin((double)angle));
  rotMat[6] = (float)sin((double)angle);
  rotMat[8] = (float)cos((double)angle);
  
  tempMat[0] = rotMat[0] * mat[0] + rotMat[1] * mat[3] + rotMat[2] * mat[6];
  tempMat[3] = rotMat[3] * mat[0] + rotMat[4] * mat[3] + rotMat[5] * mat[6];
  tempMat[6] = rotMat[6] * mat[0] + rotMat[7] * mat[3] + rotMat[8] * mat[6];

  tempMat[1] = rotMat[0] * mat[1] + rotMat[1] * mat[4] + rotMat[2] * mat[7];
  tempMat[4] = rotMat[3] * mat[1] + rotMat[4] * mat[4] + rotMat[5] * mat[7];
  tempMat[7] = rotMat[6] * mat[1] + rotMat[7] * mat[4] + rotMat[8] * mat[7];

  tempMat[2] = rotMat[0] * mat[2] + rotMat[1] * mat[5] + rotMat[2] * mat[8];
  tempMat[5] = rotMat[3] * mat[2] + rotMat[4] * mat[5] + rotMat[5] * mat[8];
  tempMat[8] = rotMat[6] * mat[2] + rotMat[7] * mat[5] + rotMat[8] * mat[8];

  memcpy(mat, tempMat, sizeof(TempFloat) * 9);
}
static void Multiply(const TempFloat *mat, TempFloat *vec)
{
  float tempVec[4];
  tempVec[0] = mat[0] * vec[0] + mat[1] * vec[1] + mat[2] * vec[2] + mat[3] * vec[3];
  tempVec[1] = mat[4] * vec[0] + mat[5] * vec[1] + mat[6] * vec[2] + mat[7] * vec[3];
  tempVec[2] = mat[8] * vec[0] + mat[9] * vec[1] + mat[10] * vec[2] + mat[11] * vec[3];
  tempVec[3] = mat[12] * vec[0] + mat[13] * vec[1] + mat[14] * vec[2] + mat[15] * vec[3];
  vec[0] = tempVec[0];
  vec[1] = tempVec[1];
  vec[2] = tempVec[2];
  vec[3] = tempVec[3];
}

/*
static void MultiplyMat(const TempFloat *mat, const TempFloat *mat2, TempFloat *outMat)
{
  outMat[0] = mat[0] * mat2[0] + mat[1] * mat2[4] + mat[2] * mat2[8] + mat[3] * mat2[12];
  outMat[4] = mat[4] * mat2[0] + mat[5] * mat2[4] + mat[6] * mat2[8] + mat[7] * mat2[12];
  outMat[8] = mat[8] * mat2[0] + mat[9] * mat2[4] + mat[10] * mat2[8] + mat[11] * mat2[12];
  outMat[12] = mat[12] * mat2[0] + mat[13] * mat2[4] + mat[14] * mat2[8] + mat[15] * mat2[12];

  outMat[1] = mat[0] * mat2[1] + mat[1] * mat2[5] + mat[2] * mat2[9] + mat[3] * mat2[13];
  outMat[5] = mat[4] * mat2[1] + mat[5] * mat2[5] + mat[6] * mat2[9] + mat[7] * mat2[13];
  outMat[9] = mat[8] * mat2[1] + mat[9] * mat2[5] + mat[10] * mat2[9] + mat[11] * mat2[13];
  outMat[13] = mat[12] * mat2[1] + mat[13] * mat2[5] + mat[14] * mat2[9] + mat[15] * mat2[13];

  outMat[2] = mat[0] * mat2[2] + mat[1] * mat2[6] + mat[2] * mat2[10] + mat[3] * mat2[14];
  outMat[6] = mat[4] * mat2[2] + mat[5] * mat2[6] + mat[6] * mat2[10] + mat[7] * mat2[14];
  outMat[10] = mat[8] * mat2[2] + mat[9] * mat2[6] + mat[10] * mat2[10] + mat[11] * mat2[14];
  outMat[14] = mat[12] * mat2[2] + mat[13] * mat2[6] + mat[14] * mat2[10] + mat[15] * mat2[14];

  outMat[3] = mat[0] * mat2[3] + mat[1] * mat2[7] + mat[2] * mat2[11] + mat[3] * mat2[15];
  outMat[7] = mat[4] * mat2[3] + mat[5] * mat2[7] + mat[6] * mat2[11] + mat[7] * mat2[15];
  outMat[11] = mat[8] * mat2[3] + mat[9] * mat2[7] + mat[10] * mat2[11] + mat[11] * mat2[15];
  outMat[15] = mat[12] * mat2[3] + mat[13] * mat2[7] + mat[14] * mat2[11] + mat[15] * mat2[15];
}
*/

static void Translate(TempFloat *mat, TempFloat *vec, TempFloat factor)
{
  mat[3] += vec[0] * factor;
  mat[7] += vec[1] * factor;
  mat[11] += vec[2] * factor;
}
void UpdateSimpleModelView(char *passTypePointer, double deltaTime)
{
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  simpleModelView->rotation += deltaTime;
  if(simpleModelView->rotation > 3.14159265f * 2.0f)
    simpleModelView->rotation -= 3.14159265f * 2.0f;
  
  if(simpleModelView->wKeyDown)
  {
    simpleModelView->camera.rotAlongX -= deltaTime;
    if(simpleModelView->camera.rotAlongX < 0.0f)
      simpleModelView->camera.rotAlongX += 3.14159265f * 2.0f;
  }
  if(simpleModelView->sKeyDown)
  {
    simpleModelView->camera.rotAlongX += deltaTime;
    if(simpleModelView->camera.rotAlongX > 3.14159265f * 2.0f)
      simpleModelView->camera.rotAlongX -= 3.14159265f * 2.0f;
  }
  if(simpleModelView->aKeyDown)
  {
    simpleModelView->camera.rotAlongY -= deltaTime;
    if(simpleModelView->camera.rotAlongY < 0.0f)
      simpleModelView->camera.rotAlongY += 3.14159265f * 2.0f;
  }
  if(simpleModelView->dKeyDown)
  {
    simpleModelView->camera.rotAlongY += deltaTime;
    if(simpleModelView->camera.rotAlongY > 3.14159265f * 2.0f)
      simpleModelView->camera.rotAlongY -= 3.14159265f * 2.0f;
  }

  if(simpleModelView->upKeyDown || simpleModelView->downKeyDown || simpleModelView->leftKeyDown || simpleModelView->rightKeyDown)
  {
    TempFloat mat[16] = {1.0f, 0.0f, 0.0f, 0.0f,
	     		 0.0f, 1.0f, 0.0f, 0.0f,
			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f, 0.0f, 0.0f, 1.0f};
    TempFloat vec[4] = { 0.0f, 0.0f, -1.0f, 1.0f};
    TempFloat vec2[4] = { 1.0f, 0.0f, 0.0f, 1.0f};

    RotateAlongY(mat, simpleModelView->camera.rotAlongY);
    RotateAlongX(mat, simpleModelView->camera.rotAlongX);
    if(simpleModelView->upKeyDown)
    {
      Multiply(mat, vec);
      simpleModelView->camera.location[0] += vec[0] * (float)deltaTime;
      simpleModelView->camera.location[1] += vec[1] * (float)deltaTime;
      simpleModelView->camera.location[2] += vec[2] * (float)deltaTime;
    }
    if(simpleModelView->downKeyDown)
    {
      Multiply(mat, vec);
      simpleModelView->camera.location[0] -= vec[0] * (float)deltaTime;
      simpleModelView->camera.location[1] -= vec[1] * (float)deltaTime;
      simpleModelView->camera.location[2] -= vec[2] * (float)deltaTime;
    }
    if(simpleModelView->leftKeyDown)
    {
      Multiply(mat, vec2);
      simpleModelView->camera.location[0] -= vec2[0] * (float)deltaTime;
      simpleModelView->camera.location[1] -= vec2[1] * (float)deltaTime;
      simpleModelView->camera.location[2] -= vec2[2] * (float)deltaTime;
    }
    if(simpleModelView->rightKeyDown)
    {
      Multiply(mat, vec2);
      simpleModelView->camera.location[0] += vec2[0] * (float)deltaTime;
      simpleModelView->camera.location[1] += vec2[1] * (float)deltaTime;
      simpleModelView->camera.location[2] += vec2[2] * (float)deltaTime;
    }
  }
}

void DrawSimpleModelView(char *passTypePointer)
{
  const TempFloat *projMatPtr, *worldMatPtr, *viewMatPtr, *normalMatPtr;
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  TempFloat lightPos[4]; 
  lightPos[4] = 1.0f;
  memcpy(lightPos, simpleModelView->light.position, 3*sizeof(float));
  projMatPtr = simpleModelView->camera.projMat;
  viewMatPtr = simpleModelView->camera.viewMat;
  worldMatPtr = simpleModelView->worldMat;
  normalMatPtr = simpleModelView->normalMat;

  Multiply(viewMatPtr, lightPos);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glBindVertexArray(simpleModelView->VAO);
  glUseProgram(simpleModelView->program);
  glUniform1i(simpleModelView->texUniformHandle, 0);
  glUniform1i(simpleModelView->normalUniformHandle, 1);
  glUniformMatrix4fv(simpleModelView->worldMatLocation, GL_TRUE, 1, worldMatPtr);
  glUniformMatrix4fv(simpleModelView->camera.projMatLocation, GL_TRUE, 1, projMatPtr);
  glUniformMatrix4fv(simpleModelView->camera.viewMatLocation, GL_TRUE, 1, viewMatPtr);
  glUniformMatrix3fv(simpleModelView->normalMatLocation, GL_TRUE, 1, normalMatPtr);
  glUniform3fv(simpleModelView->light.lightPosLocation, 1, lightPos);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, simpleModelView->texture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, simpleModelView->normalMap);
  

  glDrawElements(GL_TRIANGLES, simpleModelView->indxCount, GL_UNSIGNED_INT, 0);

  glUseProgram(0);
  glBindVertexArray(0);
}

void UpdateSimpleModelViewDrawData(char *passTypePointer)
{
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  Identity3v(simpleModelView->normalMat);
  Identity(simpleModelView->worldMat);
  RotateAlongY(simpleModelView->worldMat, simpleModelView->rotation);
  RotateAlongY3v(simpleModelView->normalMat, simpleModelView->rotation);
  Identity(simpleModelView->camera.viewMat);
  Translate(simpleModelView->camera.viewMat, simpleModelView->camera.location, -1.0f);
  RotateAlongY(simpleModelView->camera.viewMat, -simpleModelView->camera.rotAlongY);
  RotateAlongY3v(simpleModelView->normalMat, -simpleModelView->camera.rotAlongY);
  RotateAlongX(simpleModelView->camera.viewMat, -simpleModelView->camera.rotAlongX);
  RotateAlongX3v(simpleModelView->normalMat, -simpleModelView->camera.rotAlongX);

}

void SimpleModelViewKey(char *passTypePointer, int key, int action)
{
  SimpleModelView *simpleModelView = (SimpleModelView *)passTypePointer;
  if(key == GLFW_KEY_DOWN)
  {
    if(action == GLFW_PRESS)
      simpleModelView->downKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->downKeyDown = 0;
  }
  if(key == GLFW_KEY_UP)
  {
    if(action == GLFW_PRESS)
      simpleModelView->upKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->upKeyDown = 0;
  }
  if(key == GLFW_KEY_LEFT)
  {
    if(action == GLFW_PRESS)
      simpleModelView->leftKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->leftKeyDown = 0;
  }
  if(key == GLFW_KEY_RIGHT)
  {
    if(action == GLFW_PRESS)
      simpleModelView->rightKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->rightKeyDown = 0;
  }
  if(key == GLFW_KEY_A)
  {
    if(action == GLFW_PRESS)
      simpleModelView->aKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->aKeyDown = 0;
  }
  if(key == GLFW_KEY_D)
  {
    if(action == GLFW_PRESS)
      simpleModelView->dKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->dKeyDown = 0;
  }
  if(key == GLFW_KEY_W)
  {
    if(action == GLFW_PRESS)
      simpleModelView->wKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->wKeyDown = 0;
  }
  if(key == GLFW_KEY_S)
  {
    if(action == GLFW_PRESS)
      simpleModelView->sKeyDown = 1;
    else if(action == GLFW_RELEASE)
      simpleModelView->sKeyDown = 0;
  }
}
