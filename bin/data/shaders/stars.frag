#version 150

in vec4 v_position;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 fragColor;

void main (void) {
    vec2 st = v_texcoord;
    vec3 color = vec3(1.);
    float alpha = 1.;
//    alpha = 1.-smoothstep(0.492,0.5, dot(st-.5,st-.5)*2.1);
    
    fragColor = vec4(color, alpha);
}
