#version 330

in vec3 position;
in vec4 color;
in vec2 texcoord;

uniform mat4 model, view, proj;

out VertexAttrib
{
  vec2 texcoord;
  vec4 color;
} vertex;

void main()
{
    vertex.color = color;
    vertex.texcoord = texcoord;
    gl_Position = view * model * vec4(position, 1.0);
}
