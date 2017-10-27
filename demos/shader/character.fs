#version 420
in vec2 vTexCoord;
in vec3 vTanSpaceLightPos;
in vec3 vTanSpaceEyePos;
out vec4 oColor;
uniform sampler2D texSampler;
uniform sampler2D normSampler;
const vec3 ambient = vec3(0.9, 0.9, 0.9); 
const vec3 diffuse = vec3(0.01, 0.01, 0.01); 
const vec3 specularColor = vec3(1.0,1.0,1.0);
const float falloffIntensity = 0.3;
const float falloffPower = 3;
void main(void)
{

  vec3 tanSpaceNormal = normalize(texture(normSampler, vec2(vTexCoord.x, -vTexCoord.y + 1.0)).xyz * 2.0 - 1.0);

  vec3 color = texture(texSampler, vec2(vTexCoord.x, -vTexCoord.y + 1.0)).xyz;
  vec3 ambientCol = color * ambient; //Add ambient
  vec3 diffuseCol = color * max(dot(vTanSpaceLightPos, tanSpaceNormal),0);

  float specularVal = dot(tanSpaceNormal, vTanSpaceLightPos) > 0 ? pow(max(0.0, dot(reflect(-vTanSpaceLightPos, tanSpaceNormal), vTanSpaceEyePos)), 8) / 20.0 : 0;

  //Add falloff
  float lerp = 1.0 - dot(normalize(vTanSpaceEyePos), tanSpaceNormal);
  float falloff = pow(lerp, falloffPower) * falloffIntensity;
  color = ambientCol + diffuseCol + specularVal * specularColor + falloff * vec3(1.0,1.0,1.0);
  oColor = vec4(color.xyz, 1.0);
}
