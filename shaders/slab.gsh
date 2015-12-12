#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 model, view, proj;

in VertexAttrib
{
  vec2 texcoord;
  vec4 color;
} vin[];

out VertexAttrib
{
  vec2 texcoord;
  vec3 normal;
  vec3 position;
  vec4 color;
} vout;

#define outputVertex(v) tmp = v; \
	gl_Position = proj * vec4(tmp, 1.0); \
	vout.position = tmp; \
	EmitVertex();

void main()
{
	mat4 viewmodel = view * model;
	
	vec3 x = 0.5 * viewmodel[0].xyz;
    // vec3 y = 0.5 * viewmodel[1].xyz;
	vec3 y = 0.4 * viewmodel[1].xyz;
	vec3 z = 0.5 * viewmodel[2].xyz;

	vec3 pos = vec3(gl_in[0].gl_Position);
	vec3 tmp;

    vout.color = vin[0].color;

    vout.normal = vec3(0.0, 1.0, 0.0);
    outputVertex(pos - x - y - z);
    outputVertex(pos + x - y - z);
    outputVertex(pos - x - y + z);
    outputVertex(pos + x - y + z);
    EndPrimitive();
}
