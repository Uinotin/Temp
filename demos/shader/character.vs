#version 420
layout(location = 0) in vec3 pos;
uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 worldMat;
uniform mat3 normalMat;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vVert;
out vec3 vTangent;
out vec3 vBitangent;
void main(void)
{
  vTexCoord = texCoord;
  vNormal = normalMat * normal;
  vTangent = normalMat * tangent;
  vBitangent = normalMat * bitangent;
  vVert =viewMat*(worldMat*vec4(pos.x,pos.y, pos.z, 1.0));
  gl_Position = projMat*(vVert);
}
