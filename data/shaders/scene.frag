#version 110

varying vec2 uv;

uniform sampler2D texture;

void main(void) {
//    at the moment there are no textures, so I do plain white.
//    gl_FragColor = vec4(texture2D(texture, uv).rgb, 1.0);
    // alpha = emission factor.
    gl_FragColor = vec4(0.8, 0.8, 0.8, 0.0);
}
