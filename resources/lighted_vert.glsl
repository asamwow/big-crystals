#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 color;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
flat out vec4 VertexColor;

void main()
{
	gl_Position = P * V * M * vertPos;
  VertexColor = vec4(color.x, color.y, color.z, 0.f);
}
