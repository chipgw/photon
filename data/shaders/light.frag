#version 110

varying vec2 uv;

uniform vec3 color;

void main(void){
    float power = pow(max(1.0-(pow(uv.x, 2.0) + pow(uv.y, 2.0)), 0.0), 4.0) * 0.2;

    gl_FragColor = vec4((color + vec3(1.0)) * power, power);
}
