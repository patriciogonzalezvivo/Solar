#version 150

#define TAU 6.283185307179586
#define ONE_OVER_TAU .159154943
#define PI 3.1415926535
#define HALF_PI 1.57079632679

uniform float u_synodic_day;

in vec4 v_position;
in vec4 v_color;
in vec2 v_texcoord;

out vec4 fragColor;

vec2 sphereCoords(in vec2 _st, in vec3 _norm) {
    vec3 vertPoint = _norm;
    float lat = acos(dot(vec3(0., 1., 0.), _norm));
    _st.y = lat / PI;
    _st.x = (acos(dot(_norm, vec3(1, 0, 0)) / sin(lat)))*ONE_OVER_TAU;
    return _st;
}

void main () {
    vec4 color = vec4(1.);
    vec2 st = v_texcoord;
    vec2 uv = st-.5;
    
    // LIGHT
    float angle = u_synodic_day*TAU-HALF_PI; // Moon fase to radiant
    vec3 l = normalize(vec3(cos(angle),0.,sin(angle)));
    
    // PLANET
    float r = .5; // radius
    float z_in = sqrt(r*r - uv.x*uv.x - uv.y*uv.y);
    
    // NORMALS
    vec3 norm = normalize(vec3(uv.x, uv.y, z_in));
    float diffuse = smoothstep(.0,.1,clamp(dot(norm, l),0.0, 1.));
    float alpha = 1.-smoothstep(0.492,0.5, dot(st-.5,st-.5)*2.1);
    alpha = clamp(alpha,0.,1.);
    
    
    color.rgb = mix(vec3(.1), vec3(1.), diffuse);
    color.a = alpha;
    
    fragColor = color;
}
