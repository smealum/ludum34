#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

uniform mat4 model, view, proj;

in VertexAttrib
{
  vec2 texcoord;
  vec4 color;
} vin[];

out VertexAttrib
{
  vec2 texcoord;
  vec4 color;
} vout;

void main() {
	mat4 viewmodel = view * model;
	
	vec3 x = 0.5 * viewmodel[0].xyz;
	vec3 y = 0.5 * viewmodel[1].xyz;
	vec3 z = 0.5 * viewmodel[2].xyz;

	vec3 pos = vec3(gl_in[0].gl_Position);

    vout.color = vin[0].color;

    gl_Position = proj * vec4(pos - x - y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x - y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x + y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x + y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x + y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x + y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x - y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x - y + z, 1.0);
    EmitVertex();

    EndPrimitive();


    gl_Position = proj * vec4(pos + x + y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x + y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x - y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos + x - y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x - y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x - y - z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x + y + z, 1.0);
    EmitVertex();

    gl_Position = proj * vec4(pos - x + y - z, 1.0);
    EmitVertex();

    EndPrimitive();
}
