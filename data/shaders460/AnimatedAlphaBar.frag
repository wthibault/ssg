#version 460
// AnimatedAlphaBar - slides a bar of alpha>0 along texture s with time
in  vec4 color;
in  vec2 uv;

uniform float Time;

out vec4 fragColor;

void main() 
{
  float freq = 0.5;
  float pw = 0.05;
  const float TWOPI = 2.0 * 3.15159;

  fragColor = color;

  //  fragColor.a = max ( 0.0, sin ( TWOPI * freq * Time + uv.s ) );
  fragColor.a = fract(freq*Time-uv.s) < pw ? 1.0 : 0.4;

} 

