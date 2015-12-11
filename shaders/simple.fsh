#version 330

in VertexAttrib
{
  vec2 texcoord;
  vec4 color;
} vin;

out vec4 out_color;

void main()
{
    out_color = vin.color;
}
