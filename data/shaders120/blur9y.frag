#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;


void main(void) {
  // blur along y 
  float ystep = 1.0/width;
  vec4 sum = vec4(0.0);

  // '0.22702703', '0.19459459', '0.12162162', '0.05405405', '0.01621622'
  sum  = texture2D(texture1, vec2 ( uv.x, uv.y - 4.0*ystep )) * 0.01621622;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y - 3.0*ystep )) * 0.05405405;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y - 2.0*ystep )) * 0.12162162;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y - 1.0*ystep )) * 0.19459459;
  sum += texture2D(texture1, uv) * 0.22702703;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y + 1.0*ystep )) * 0.19459459;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y + 2.0*ystep )) * 0.12162162;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y + 3.0*ystep )) * 0.05405405;
  sum += texture2D(texture1, vec2 ( uv.x, uv.y + 4.0*ystep )) * 0.01621622;
  gl_FragColor = sum;

}
