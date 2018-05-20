uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

attribute vec4 position;
attribute vec4 color;
attribute vec3 normal;
attribute vec2 texcoord;

varying vec4 v_position;
varying vec4 v_color;
varying vec2 v_texcoord;
varying vec3 v_normal;

void main() {
    v_texcoord  = texcoord;
    v_color     = color;
    v_normal    = normal;
    gl_Position = modelViewProjectionMatrix * position;
}

