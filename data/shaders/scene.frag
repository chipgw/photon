#version 110

varying vec2 uv;
varying vec2 screen;

uniform sampler2D texture;
uniform sampler2D texture_light;

void main(void) {
    vec4 color = texture2D(texture, uv);
    vec4 light_color = texture2D(texture_light, screen);
    color.rgb *= light_color.rgb + vec3(0.1);
    gl_FragColor = vec4(color * color.a);
}
