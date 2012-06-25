#version 130

in vec2 vertex;
in vec2 texcoord;
in vec4 colors;

out vec2 tex;

uniform mat4 mvp;

void main()
{
    vec4 p = mvp * vec4(vertex, 0.0f, 1.0f);
    gl_Position = p;
    tex = texcoord;
}
