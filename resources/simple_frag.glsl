#version 330 core 
flat in vec4 VertexColor;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 bloomColor;

void main()
{
   color = VertexColor;
   bloomColor = vec4(0.f, 0.f, 0.f, 1.f);
}
