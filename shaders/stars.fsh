#version 330

in VertexAttribFrag
{
	vec4 color;
} vin;

out vec4 out_color;

void main()
{
	out_color = vin.color;
}
