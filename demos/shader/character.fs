#version 420
in vec2 vTexCoord;
in vec3 vNormal;
in vec4 vVert;
in vec3 vTangent;
in vec3 vBitangent;
out vec4 oColor;
uniform sampler2D texSampler;
uniform sampler2D normSampler;
uniform vec3 lightPos;
const vec3 ambient = vec3(0.9, 0.9, 0.9); 
const vec3 diffuse = vec3(0.01, 0.01, 0.01); 
const vec3 specularColor = vec3(1.0,1.0,1.0);
const float falloffIntensity = 0.3;
void main(void)
{
  mat3 TBN = mat3(vTangent, vBitangent, vNormal);
  vec3 tanSpaceLightPos = normalize(TBN * (lightPos - vVert.xyz));
  vec3 tanSpaceEyePos = normalize(TBN * -vVert.xyz);
  vec3 tanSpaceNormal = normalize(texture(normSampler, vec2(vTexCoord.x, -vTexCoord.y + 1.0)).xyz * 2.0 - 1.0);

  vec3 color = texture(texSampler, vec2(vTexCoord.x, -vTexCoord.y + 1.0)).xyz;
  vec3 ambientCol = color * ambient; //Add ambient
  vec3 diffuseCol = color * max(dot(tanSpaceLightPos, tanSpaceNormal),0);

  float specularVal = dot(tanSpaceNormal, tanSpaceLightPos) > 0 ? pow(max(0.0, dot(reflect(-tanSpaceLightPos, tanSpaceNormal), tanSpaceEyePos)), 8) / 20.0 : 0;

  //Add falloff
  float lerp = 1.0 - dot(normalize(tanSpaceEyePos), tanSpaceNormal);
  float falloff = pow(lerp, 3) * falloffIntensity;
  color = ambientCol + diffuseCol + specularVal * specularColor + falloff * vec3(1.0,1.0,1.0);
  oColor = vec4(color.xyz, 1.0);
}
