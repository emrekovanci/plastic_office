#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

layout(std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

uniform vec2 uv_scale;
uniform vec2 uv_offset;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord * uv_scale + uv_offset;
}

