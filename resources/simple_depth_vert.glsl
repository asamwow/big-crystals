#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 color;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
flat out vec4 VertexColor;
out float depth;

void main()
{
	gl_Position = P * V * M * vertPos;
  depth = gl_Position.z / 50000.f;
  VertexColor = vec4(color, 1.f);
}
