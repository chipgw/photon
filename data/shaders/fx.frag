#version 110

varying vec2 uv;
varying vec2 screen;

uniform sampler2D texture;

uniform float fac;

void main(void) {
    vec4 color = texture2D(texture, uv);
    gl_FragColor = vec4(color * color.a * fac);
}
