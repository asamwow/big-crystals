#version 330 core 
flat in vec4 VertexColor;
in float depth;
uniform float emission;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 bloomColor;

void main()
{
   color = VertexColor;
   if (emission > 0) {
      bloomColor = color;
   } else {
      bloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
   }
   // if (depth == 0.f) {
   //    depthMap = 0.f;
   // }  
   // depthMap = vec4(vec3(depth), 1.0f);
   // depthMap = depth;
   // depthMap = vec4(vec3(max(0.f,depth)), 1.f);
}
