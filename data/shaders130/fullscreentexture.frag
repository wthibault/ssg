#version 130

uniform sampler2D texture1;

varying vec2 uv;


void main(void) {
    gl_FragColor = texture(texture1,uv);
}
