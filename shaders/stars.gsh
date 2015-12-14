#version 330

#define CHAR_RESOLUTION (1.0 / 128)

layout(points) in;
layout(points, max_vertices = 1) out;

uniform float aspect;
uniform float t;

in VertexAttrib
{
	vec4 color;
	float period, phase;
} vin[];

out VertexAttribFrag
{
	vec4 color;
} vout;

void main()
{
    vout.color = vin[0].color;
    vout.color.w = (sin(t * vin[0].period * 2.0 + vin[0].phase * 3.0) + 1.0) * 0.4 + 0.2;

    gl_PointSize = 1.0; // * sin(t * 6.0f + gl_in[0].gl_Position.x + gl_in[0].gl_Position.y) * 10.0f;
    gl_Position = gl_in[0].gl_Position + vec4(vec2(sin(t * vin[0].period), cos((t + vin[0].phase) * vin[0].period)) * vin[0].phase * 0.05, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();
}
