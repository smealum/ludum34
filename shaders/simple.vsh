#version 330

in vec3 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;

uniform mat4 model, view, proj;

out VertexAttrib
{
  vec2 texcoord;
  vec3 normal;
  vec3 position;
  vec4 color;
} vertex;

void main()
{
	vec3 tmp = vec3(view * model * vec4(position, 1.0));
    vertex.color = color;
    vertex.texcoord = texcoord;
    vertex.normal = normal;
    vertex.position = tmp;
    gl_Position = proj * vec4(tmp, 1.0);
}
