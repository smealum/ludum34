#version 150 core

in vec3 position;
in vec4 color;
in vec2 texcoord;

uniform mat4 model, view, proj;

out vec4 frag_color;
out vec2 frag_texcoord;

void main()
{
    frag_color = color;
    frag_texcoord = texcoord;
    gl_Position = proj * view * model * vec4(position, 1.0);
}
