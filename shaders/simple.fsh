#version 150 core

in vec4 frag_color;
in vec2 frag_texcoord;

out vec4 out_color;

void main()
{
    out_color = frag_color;
}
