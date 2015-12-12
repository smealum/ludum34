#version 330

in VertexAttribFrag
{
  vec2 texcoord;
} vin;

uniform sampler2D texture;

out vec4 out_color;

void main()
{
	out_color = texture2D( texture, vec2(vin.texcoord.x, vin.texcoord.y) ).zyxw;
	// out_color = vec4(1.0, 0.0, 0.0, 1.0);
}
