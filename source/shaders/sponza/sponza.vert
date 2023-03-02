#version 450

layout(location = 0) in vec4 pos;
layout(location = 1) in vec3 nol;
layout(location = 2) in vec2 uv;

layout(binding = 0) uniform global{
    mat4 mvp;
} ubo;


void main() {
    gl_Position = mvp * pos;
}