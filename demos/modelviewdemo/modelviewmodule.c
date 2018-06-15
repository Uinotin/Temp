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
      assert(stringIndex < 4096);
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

static void GenTangents(char *vertData, const char *indexData, const Attrib *indexAttrib, const VertAttrib *vertAttrib, const VertAttrib *texCoordAttrib, const VertAttrib *normAttrib, VertAttrib *tanAttrib, VertAttrib *bitanAttrib)
{
  int i;
  TempSizei nIndices, nVerts;
  	
  const TempUInt *__restrict__ indices;
  const TempFloat *__restrict__ texCoords, *__restrict__ norms, *__restrict__ verts;
  TempFloat *__restrict__ bitangents, *__restrict__ tangents;
  TempSizei texStride, normStride, vertStride, bitanStride, tanStride;
  nIndices = indexAttrib->arraySize;
  nVerts = vertAttrib->attrib.arraySize;
 
  verts = (TempFloat *)(vertData + vertAttrib->attrib.offset);
  norms = (TempFloat *)(vertData + normAttrib->attrib.offset);
  texCoords = (TempFloat *)(vertData + texCoordAttrib->attrib.offset);
  tangents = (TempFloat *)(vertData + tanAttrib->attrib.offset);
  bitangents = (TempFloat *)(vertData + bitanAttrib->attrib.offset);
  indices = (TempUInt *)(indexData + indexAttrib->offset);
  vertStride = vertAttrib->stride / sizeof(TempFloat);
  texStride = texCoordAttrib->stride / sizeof(TempFloat);
  normStride = normAttrib->stride / sizeof(TempFloat);
  tanStride = tanAttrib->stride / sizeof(TempFloat);
  bitanStride = bitanAttrib->stride / sizeof(TempFloat);
  for(i = 0; i < nVerts; ++i)
  {
    tangents[i * tanStride + 0] = 0.0f;
    tangents[i * tanStride + 1] = 0.0f;
    tangents[i * tanStride + 2] = 0.0f;
    bitangents[i * bitanStride + 0] = 0.0f;
    bitangents[i * bitanStride + 1] = 0.0f;
    bitangents[i * bitanStride + 2] = 0.0f;
  }

  for(i = 0; i < nIndices/3; ++i)
  {
    const TempFloat *v0, *v1, *v2, *uv0, *uv1, *uv2;
    TempFloat deltaUV1[2], deltaUV2[2], deltaPos1[3], deltaPos2[3];
    TempFloat r, coordX, coordY, coordZ;
    v0 = verts + indices[0] * vertStride;
    v1 = verts + indices[1] * vertStride;
    v2 = verts + indices[2] * vertStride;
    uv0 = texCoords + indices[0] * texStride;
    uv1 = texCoords + indices[1] * texStride;
    uv2 = texCoords + indices[2] * texStride;
  
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
    coordX = (deltaPos1[0] * deltaUV2[1] - deltaPos2[0] * deltaUV1[1]) * r;
    coordY = (deltaPos1[1] * deltaUV2[1] - deltaPos2[1] * deltaUV1[1]) * r;
    coordZ = (deltaPos1[2] * deltaUV2[1] - deltaPos2[2] * deltaUV1[1]) * r;

    tangents[indices[0] * tanStride + 0] += coordX;
    tangents[indices[0] * tanStride + 1] += coordY;
    tangents[indices[0] * tanStride + 2] += coordZ;
    tangents[indices[1] * tanStride + 0] += coordX;
    tangents[indices[1] * tanStride + 1] += coordY;
    tangents[indices[1] * tanStride + 2] += coordZ;
    tangents[indices[2] * tanStride + 0] += coordX;
    tangents[indices[2] * tanStride + 1] += coordY;
    tangents[indices[2] * tanStride + 2] += coordZ;

    coordX = (deltaPos2[0] * deltaUV1[0] - deltaPos1[0] * deltaUV2[0]) * r;
    coordY = (deltaPos2[1] * deltaUV1[0] - deltaPos1[1] * deltaUV2[0]) * r;
    coordZ = (deltaPos2[2] * deltaUV1[0] - deltaPos1[2] * deltaUV2[0]) * r;

    bitangents[indices[0] * bitanStride + 0] += coordX;
    bitangents[indices[0] * bitanStride + 1] += coordY;
    bitangents[indices[0] * bitanStride + 2] += coordZ;
    bitangents[indices[1] * bitanStride + 0] += coordX;
    bitangents[indices[1] * bitanStride + 1] += coordY;
    bitangents[indices[1] * bitanStride + 2] += coordZ;
    bitangents[indices[2] * bitanStride + 0] += coordX;
    bitangents[indices[2] * bitanStride + 1] += coordY;
    bitangents[indices[2] * bitanStride + 2] += coordZ;

    indices += 3;
  }

  for(i = 0; i < nVerts; ++i)
  {
    float len;
    float dot = tangents[0] * norms[0] + tangents[1] * norms[1] + tangents[2] * norms[2];
    tangents[0] -= norms[0] * dot; 
    tangents[1] -= norms[1] * dot; 
    tangents[2] -= norms[2] * dot; 

    len = (float)sqrt(
  		     (double)(tangents[0]*tangents[0] +
                              tangents[1]*tangents[1] +
			      tangents[2]*tangents[2]));
    tangents[0] /= len;
    tangents[1] /= len;
    tangents[2] /= len;
	  
    dot = bitangents[0] * norms[0] + bitangents[1] * norms[1] + bitangents[2] * norms[2];
    bitangents[0] -= norms[0] * dot; 
    bitangents[1] -= norms[1] * dot; 
    bitangents[2] -= norms[2] * dot; 
	  
    len = (float)sqrt(
		     (double)(bitangents[0]*bitangents[0] +
                              bitangents[1]*bitangents[1] +
			      bitangents[2]*bitangents[2]));
    bitangents[0] /= len;
    bitangents[1] /= len;
    bitangents[2] /= len;

    norms += normStride;
    tangents += tanStride;
    bitangents += bitanStride;
  }
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

static void BindVertexAttrib(VertAttrib *attrib)
{
  glVertexAttribPointer(attrib->index, attrib->size, attrib->attrib.type, GL_FALSE, attrib->stride, (char *)0 + attrib->attrib.offset);
  glEnableVertexAttribArray(attrib->index);
}

static void Translate(TempFloat *mat, TempFloat *vec, TempFloat factor)
{
  mat[3] += vec[0] * factor;
  mat[7] += vec[1] * factor;
  mat[11] += vec[2] * factor;
}

void LoadDrawModelView(ProgramTreeNode *drawNode)
{
  TempUInt handle, vHandle, fHandle;
  DrawModelViewData *locals;
  drawNode->children = 0;
  drawNode->nChildren = 0;
  drawNode->locals = (void *)malloc(sizeof(DrawModelViewData));
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
    VertAttrib *attribs;
    int nVertAttribs;
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
      Attrib indexAttrib = {.type = GL_UNSIGNED_INT, .arraySize = nIndices, .offset = 0};
      nVertAttribs = 5;
      attribs = (VertAttrib *)malloc(nVertAttribs*sizeof(VertAttrib));
      attribs[0].index = 0;
      attribs[0].size = 3;
      attribs[0].attrib.type = GL_FLOAT;
      attribs[0].stride = 8 * sizeof(TempFloat);
      attribs[0].attrib.offset = 0 * sizeof(TempFloat);
      attribs[0].attrib.arraySize = nVerts;
      attribs[1].index = 1;
      attribs[1].size = 2;
      attribs[1].attrib.type = GL_FLOAT;
      attribs[1].stride = 8 * sizeof(TempFloat);
      attribs[1].attrib.offset = 3 * sizeof(TempFloat);
      attribs[2].index = 2;
      attribs[2].size = 3;
      attribs[2].attrib.type = GL_FLOAT;
      attribs[2].stride = 8 * sizeof(TempFloat);
      attribs[2].attrib.offset = 5 * sizeof(TempFloat);
      attribs[3].index = 3;
      attribs[3].size = 3;
      attribs[3].attrib.type = GL_FLOAT;
      attribs[3].stride = 3 * sizeof(TempFloat);
      attribs[3].attrib.offset = nVerts * 8 * sizeof(TempFloat);
      attribs[4].index = 4;
      attribs[4].size = 3;
      attribs[4].attrib.type = GL_FLOAT;
      attribs[4].stride = 3 * sizeof(TempFloat);
      attribs[4].attrib.offset = nVerts * 11 * sizeof(TempFloat);

      GenTangents(data, (const char *)(data + nVerts * 14 * sizeof(float)),
	          &indexAttrib,
	          attribs,
		  attribs + 1,
		  attribs + 2,
		  attribs + 3,
		  attribs + 4);
      nVertAttribs = 5;
    }
    {
      TempUInt VAO, VBO, IBO;
      int i;
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, nVerts * 14 * sizeof(float), data, GL_STATIC_DRAW);
      glGenBuffers(1, &IBO);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), data + nVerts * 14 * sizeof(float), GL_STATIC_DRAW);
      for (i = 0; i < nVertAttribs; ++i)
	BindVertexAttrib(attribs + i);
      /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), 0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 3 * sizeof(TempFloat));
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(TempFloat), (char *)0 + 5 * sizeof(TempFloat));
      //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + nVerts * 8 * sizeof(TempFloat));
      //glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3*sizeof(TempFloat), (char *)0 + nVerts * 11 * sizeof(TempFloat));
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      //glEnableVertexAttribArray(3);
      //glEnableVertexAttribArray(4);
      */
      glBindVertexArray(0);

      locals->VAO = VAO;
      locals->VBO = VBO;
      locals->IBO = IBO;
    }
    free(attribs);
    free(data);
  }

  locals->texture = LoadTGA("texture_001.tga", 1);
  locals->normalMap = LoadTGA("normals_05.tga", 0);
  Identity(locals->projMat);
  Perspective(locals->projMat, 6.1, 0.1f, 10.0f, 1280.0f/720.0f);
  Identity(locals->viewMat);
  {
    TempFloat vec[] = {0.0f, -0.5f, -0.7f};
    Translate(locals->viewMat, vec, 1.0f);
  }
  Identity(locals->worldMat);
  Identity3v(locals->normalMat);

  glClearColor(0.129f, 0.129f, 0.129f, 1.0f);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

void DrawModelView(void *locals, void *out)
{
  const TempFloat *projMatPtr, *worldMatPtr, *viewMatPtr, *normalMatPtr;
  DrawModelViewData *drawData = (DrawModelViewData *)(locals);
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
