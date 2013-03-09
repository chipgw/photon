#version 110

varying vec2 uv;

uniform sampler2D texture;

uniform vec4 color;

void main(void) {
    vec4 texture = texture2D(texture, uv);
    gl_FragColor = vec4((color * texture.a) + texture);
}
