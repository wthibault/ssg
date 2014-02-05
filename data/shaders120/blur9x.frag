#version 120

uniform sampler2D texture1;
uniform float width;
uniform float height;

varying vec2 uv;


void main(void) {
  // blur along x 
  float step = 1.0/width;
  vec4 sum = vec4(0.0,0.0,0.0,1.0);
  // sum  = texture2D(texture1, vec2 ( uv.x - 4.0*xstep, uv.y )) * 0.05;
  // sum += texture2D(texture1, vec2 ( uv.x - 3.0*xstep, uv.y )) * 0.09;
  // sum += texture2D(texture1, vec2 ( uv.x - 2.0*xstep, uv.y )) * 0.12;
  // sum += texture2D(texture1, vec2 ( uv.x - 1.0*xstep, uv.y )) * 0.15;
  // sum += texture2D(texture1, uv) * 0.16;
  // sum += texture2D(texture1, vec2 ( uv.x + 1.0*xstep, uv.y )) * 0.15;
  // sum += texture2D(texture1, vec2 ( uv.x + 2.0*xstep, uv.y )) * 0.12;
  // sum += texture2D(texture1, vec2 ( uv.x + 3.0*xstep, uv.y )) * 0.09;
  // sum += texture2D(texture1, vec2 ( uv.x + 4.0*xstep, uv.y )) * 0.05;
  // gl_FragColor = sum;

  // '0.22702703', '0.19459459', '0.12162162', '0.05405405', '0.01621622'

  float x = max ( 0.0, uv.x-4.0*step );
  sum = texture2D(texture1, vec2 ( x, uv.y ))  * 0.01621622;
  x = max ( 0.0, uv.x-3.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.05405405;
  x = max ( 0.0, uv.x-2.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.12162162;
  x = max ( 0.0, uv.x-1.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.19459459;
  sum += texture2D(texture1, uv) * 0.22702703;
  x = min ( 1.0, uv.x+1.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.19459459;
  x = min ( 1.0, uv.x+2.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.12162162;
  x = min ( 1.0, uv.x+3.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.05405405;
  x = min ( 1.0, uv.x+4.0*step );
  sum += texture2D(texture1, vec2 ( x, uv.y )) * 0.01621622;

  // sum  = texture2D(texture1, vec2 ( uv.x - 4.0*xstep, uv.y )) * 0.01621622;
  // sum += texture2D(texture1, vec2 ( uv.x - 3.0*xstep, uv.y )) * 0.05405405;
  // sum += texture2D(texture1, vec2 ( uv.x - 2.0*xstep, uv.y )) * 0.12162162;
  // sum += texture2D(texture1, vec2 ( uv.x - 1.0*xstep, uv.y )) * 0.19459459;
  // sum += texture2D(texture1, uv) * 0.22702703;
  // sum += texture2D(texture1, vec2 ( uv.x + 1.0*xstep, uv.y )) * 0.19459459;
  // sum += texture2D(texture1, vec2 ( uv.x + 2.0*xstep, uv.y )) * 0.12162162;
  // sum += texture2D(texture1, vec2 ( uv.x + 3.0*xstep, uv.y )) * 0.05405405;
  // sum += texture2D(texture1, vec2 ( uv.x + 4.0*xstep, uv.y )) * 0.01621622;
  gl_FragColor = vec4(sum.xyz, 1.0);

}
