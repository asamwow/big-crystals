#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 color;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightV;
uniform sampler2D shadows;
flat out vec4 VertexColor;

void main() {
   gl_Position = P * V * M * vertPos;
   float depth = gl_Position.z / 50000.f;

   vec4 FragPosLightSpace = P*lightV*M*vertPos;
   vec3 projCoords = FragPosLightSpace.xyz/ FragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;
   float closestDepth = texture(shadows, projCoords.xy).r;
   if (closestDepth > 0.f) {
      VertexColor = vec4(0.f, 0.f, 0.f, 1.f);
   } else {
      VertexColor = vec4(max(0.f, color.x - depth), max(0.f, color.y - depth),
                      max(0.f, color.z - depth), 1.f);
   }
   // VertexColor = vec4(vec3(closestDepth*1000), 1.f);
   // VertexColor = vec4(projCoords.xyz, 1.f);
}
