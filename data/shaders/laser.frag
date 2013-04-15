#version 110

varying vec2 uv;

uniform vec3 color;

void main(void) {
    float power = pow(2.0 * uv.x, 4.0);

    gl_FragColor = vec4(color * power, power);
}
