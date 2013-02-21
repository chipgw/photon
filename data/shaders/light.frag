#version 110

varying vec2 uv;
varying vec2 screen;

uniform sampler2D texture;

void main(void) {
    float power = pow((1.0 - abs(uv.x)) * uv.y * 0.5, 3.0) * 8.0;

    vec4 color = texture2D(texture, screen);

    vec3 emit = color.xyz * color.a;
    vec3 lit = color.xyz * power * (1.0 - color.a);

    gl_FragColor = vec4(emit+lit, 1.0);
}
