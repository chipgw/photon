#version 110

uniform float aspect;
uniform float zoom;

in vec2 in_location;
in vec2 in_uv;

varying vec2 uv;
varying vec2 screen;

void main(void) {
    vec2 location = in_location * zoom;

    if(aspect > 1.0){
        location.x /= aspect;
    }else if(aspect < 1.0){
        location.y *= aspect;
    }

    gl_Position = vec4(location, 0.0, 1.0);

    screen = (location + vec2(1.0,1.0))*0.5;

    uv = in_uv;
}
