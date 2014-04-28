#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;


void main(void) {
  // zoom towards point at infinity
  float step = 1.0/width;
  vec2 center = vec2( 0.5, 0.5);
  vec2 frag = vec2( gl_FragCoord.x / width, 
		    gl_FragCoord.y / height );
  frag -= center;
  frag *= 1.0 / 1.008;
  frag += center;
  vec2 src = frag;

  vec4 sum = texture2D(texture1, src);


  gl_FragColor = vec4(sum.xyz, 1.0);

}
