uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

attribute vec4 color;
attribute vec4 position;
attribute vec2 texcoord;

varying vec4 v_position;
varying vec4 v_color;
varying vec2 v_texcoord;

void main() {
    v_texcoord  = texcoord;
    v_color     = color;
    gl_Position = modelViewProjectionMatrix * position;
}
