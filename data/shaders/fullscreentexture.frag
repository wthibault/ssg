#version 150

uniform sampler2D texture1;

in vec2 uv;

out vec4 fragColor;

void main(void) {
    fragColor = texture(texture1,uv);
}
