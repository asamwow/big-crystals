#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 color;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
flat out vec4 VertexColor;

void main() {
   gl_Position = P * V * M * vertPos;
   // worldPosition = gl_Position;
   // VertexColor = vec4(vec3(gl_Position.z/1000.f), 1.f);
   float depth = gl_Position.z / 50000.f;
   VertexColor = vec4(max(0.f, color.x - depth), max(0.f, color.y - depth),
                      max(0.f, color.z - depth), 1.f);
   // VertexColor = vec4(vec3(depth), 1.f);
}
