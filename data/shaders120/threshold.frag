#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;

//out vec4 fragColor;

void main(void) {
  float s =  sign(texture2D(texture1,uv).r);
  gl_FragColor = vec4(s,s,s,1.0);
}
