#version 110

varying vec2 uv;

uniform sampler2D texture;

void main(void) {
    gl_FragColor = vec4(texture2D(texture, abs((uv * 2.0) - 1.0)).rgb, 1.0);
}
