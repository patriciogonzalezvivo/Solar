#version 150

in vec4 v_position;
in vec4 v_color;
in vec3 v_normal;
in vec2 v_texcoord;

uniform sampler2D u_diffuse;

out vec4 fragColor;

void main () {
    vec3 color = vec3(1., 0., 0.);
    vec2 st = v_texcoord;
//    st.x = 1.-st.x;
    st.y = 1.-st.y;
    
//    color = v_normal * .5 + .5;
    color = texture(u_diffuse, st).rgb;
    color *= .5;
    color += .25;
    
    fragColor = vec4(color, 1.);
}
