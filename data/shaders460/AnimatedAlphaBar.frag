#version 460
// AnimatedAlphaBar - slides a bar of alpha>0 along texture s with time
in  vec4 color;
in  vec2 uv;

uniform float Time;

out vec4 fragColor;

void main() 
{
  float freq = 2.0; // temporal freq
  float pw = 0.5;   // pulsewidth
  float repeats = 20.0;
  const float TWOPI = 2.0 * 3.15159;

  fragColor = color;

  //  fragColor.a = max ( 0.0, sin ( TWOPI * freq * Time + uv.s ) );
  fragColor.a = fract( freq * Time - uv.s * repeats ) < pw ? 1.0 : 0.4;
  if (fragColor.a < 0.5)
    discard;
} 

