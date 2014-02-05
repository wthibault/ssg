#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;

//out vec4 fragColor;

void main(void) {
  // blur!
  float xstep = 1.0/width;
  float ystep = 1.0/height;
  gl_FragColor = 1.0/16.0 * (texture2D(texture1,uv+vec2(0.0,0.0)) * 4
			  + texture2D(texture1,uv+vec2(xstep,0.0)) * 2
			  + texture2D(texture1,uv+vec2(0.0,ystep)) * 2
			  + texture2D(texture1,uv+vec2(-xstep,0.0)) * 2
			  + texture2D(texture1,uv+vec2(0.0,-ystep)) * 2
			  + texture2D(texture1,uv+vec2(xstep,ystep)) * 1
			  + texture2D(texture1,uv+vec2(xstep,-ystep)) * 1
			  + texture2D(texture1,uv+vec2(-xstep,ystep)) * 1
			  + texture2D(texture1,uv+vec2(-xstep,-ystep)) * 1);
}
