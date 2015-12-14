#version 330

in vec2 position;
in vec4 color;
in float period;
in float phase;

out VertexAttrib
{
	vec4 color;
	float period, phase;
} vertex;

void main()
{
	vertex.color = color;
	vertex.period = period;
	vertex.phase = phase;
	gl_Position = vec4(position, 0.0, 1.0);
}
