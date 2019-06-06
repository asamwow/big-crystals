#version 330 core 
flat in vec4 VertexColor;
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
}
