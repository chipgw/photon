#version 110

varying vec2 uv;

void main(void) {
    float power = pow(1.5*(1.0 - abs(uv.x)), 4.0);

    vec3 color = normalize(vec3(0.9, 0.2, 0.1));

    gl_FragColor = vec4(color * power, power);
}
