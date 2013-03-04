#version 110

uniform float aspect;

attribute vec2 in_location;
attribute vec2 in_uv;

varying vec2 uv;

void main(void) {
    vec2 location = in_location;

    if(aspect > 1.0){
        location.x /= aspect;
    }else if(aspect < 1.0){
        location.y *= aspect;
    }

    gl_Position = vec4(location, 0.0, 1.0);

    uv = in_uv;
}
