#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;

const float PI = 3.1415926;

void main(void) {
  // rotate about center
  float step = 1.0/width;
  vec2 center = vec2( 0.5, 0.5);
  vec2 frag = vec2( gl_FragCoord.x / width, gl_FragCoord.y / height );
  frag -= center;
  float angle =  2.0 * PI / 500.0 ;
  float c = cos (angle);
  float s = sin (angle);
  vec2  src;
  src.x = frag.x * c - frag.y * s;
  src.y = frag.x * s + frag.y * c;
  
  src += center;

  vec4 sum = texture2D(texture1, src);


  gl_FragColor = vec4(sum.xyz, 1.0);

}
