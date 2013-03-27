#version 120

uniform sampler2D texture1;

varying vec2 uv;


void main(void) {
    gl_FragColor = texture2D(texture1,uv);
}
