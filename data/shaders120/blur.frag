#version 150

uniform sampler2D texture1;

in vec2 uv;

out vec4 fragColor;

void main(void) {
  //    fragColor = texture(texture1,uv);
  // blur!
  const float step = 1.0/1024.0;// !!! assumes 1k x 1k texture!
  fragColor = 1.0/16.0 * (texture(texture1,uv+vec2(0.0,0.0)) * 4
			  + texture(texture1,uv+vec2(step,0.0)) * 2
			  + texture(texture1,uv+vec2(0.0,step)) * 2
			  + texture(texture1,uv+vec2(-step,0.0)) * 2
			  + texture(texture1,uv+vec2(0.0,-step)) * 2
			  + texture(texture1,uv+vec2(step,step)) * 1
			  + texture(texture1,uv+vec2(step,-step)) * 1
			  + texture(texture1,uv+vec2(-step,step)) * 1
			  + texture(texture1,uv+vec2(-step,-step)) * 1);
}
