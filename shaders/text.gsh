#version 330

#define CHAR_RESOLUTION (1.0 / 128)

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 model, view, proj;
uniform float CHAR_SIZE, aspect;

in VertexAttrib
{
  float character;
} vin[];

out VertexAttribFrag
{
  vec2 texcoord;
} vout;

#define outputVertex(offset) gl_Position = gl_in[0].gl_Position + vec4(offset.x * CHAR_SIZE, offset.y * CHAR_SIZE * aspect, 0.0, 0.0); \
    vout.texcoord = vec2(offset.x, (vin[0].character - 32.0 + (1.0 - offset.y)) * CHAR_RESOLUTION); \
    EmitVertex();

void main()
{
    outputVertex(vec4(0.0, 0.0, 0.0, 0.0));
    outputVertex(vec4(0.0, 1.0, 0.0, 0.0));
    outputVertex(vec4(1.0, 0.0, 0.0, 0.0));
    outputVertex(vec4(1.0, 1.0, 0.0, 0.0));
    EndPrimitive();
}
