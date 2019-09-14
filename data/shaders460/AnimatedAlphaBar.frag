#version 460
// GouraudShading.frag = passthrough interpolated color.
in  vec4 color;
in  vec2 uv;

uniform float Time;

out vec4 fragColor;

void main() 
{
  float freq = 1.0;
  const float TWOPI = 2.0 * 3.15159;
  fragColor = color;
  fragColor.a = max ( 0.0, sin ( TWOPI * freq * Time + uv.s ) );
  //  gl_FragColor = color;
} 

