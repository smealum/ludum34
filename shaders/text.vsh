#version 330

in float character;

uniform vec2 position;
uniform float CHAR_SIZE;

out VertexAttrib
{
  float character;
} vertex;

void main()
{
  vertex.character = character;
  gl_Position = vec4(position + vec2(gl_VertexID * CHAR_SIZE, 0.0), 0.0, 1.0);
}
