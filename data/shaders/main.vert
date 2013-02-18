#version 110

in vec2 in_location;
in vec2 in_uv;

varying vec2 uv;

void main(void) {
    vec4 position = vec4(in_location, 0.0, 1.0);
    gl_Position = position;

    uv = in_uv;
}
