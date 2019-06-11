#version 330 core 
in float depth;
flat in vec4 VertexColor;

layout (location = 2) out vec4 depthMap;

void main()
{
   depthMap = vec4(vec3(max(0.f,depth)), 1.f);
   if (depthMap.x != 0.f) {
      depthMap.x = 1.f;
   }
   // depthMap = VertexColor;
}
