#version 330

in vec3 position;
in vec4 color;
in vec2 texcoord;
in float fall_time;

uniform mat4 model, view, proj;

out VertexAttrib
{
  vec2 texcoord;
  vec4 color;
  float fall_time;
} vertex;

void main()
{
    vertex.color = color;
    vertex.texcoord = texcoord;
    vertex.fall_time = fall_time;
    gl_Position = view * model * vec4(position, 1.0);
}
