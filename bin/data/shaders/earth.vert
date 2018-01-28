#version 150

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;

out vec4 v_position;
out vec4 v_color;
out vec2 v_texcoord;
out vec3 v_normal;

void main() {
    v_texcoord  = texcoord;
    v_color     = color;
    v_normal    = normal;
    gl_Position = modelViewProjectionMatrix * position;
}

