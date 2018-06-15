#version 420
uniform mat4 projMat;
uniform mat4 viewMat;
uniform mat4 worldMat;
uniform mat3 normalMat;
uniform vec3 lightPos;
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
out vec2 vTexCoord;
out vec3 vTanSpaceLightPos;
out vec3 vTanSpaceEyePos;
void main(void)
{
  vTexCoord = texCoord;

  mat3 TBN = mat3(normalMat * tangent, normalMat * bitangent, normalMat * normal);

  vec4 vertPos = worldMat*vec4(pos.x,pos.y, pos.z, 1.0);
  vTanSpaceLightPos = normalize(TBN * (lightPos - vertPos.xyz/vertPos.w));
  vertPos = viewMat * vertPos;
  vTanSpaceEyePos = normalize(TBN * -(vertPos.xyz/vertPos.w ));

  gl_Position = projMat*vertPos;
}
