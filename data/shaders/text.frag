#version 110

varying vec2 uv;

uniform sampler2D texture;

uniform vec4 color;

void main(void) {
    float alpha = texture2D(texture, uv).a;
    gl_FragColor = vec4(color * alpha);
}
