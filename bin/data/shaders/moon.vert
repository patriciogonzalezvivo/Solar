#version 150

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 color;
in vec4 position;
in vec2 texcoord;

out vec4 v_position;
out vec4 v_color;
out vec2 v_texcoord;

void main() {
    v_texcoord  = texcoord;
    v_color     = color;
    gl_Position = modelViewProjectionMatrix * position;
}
