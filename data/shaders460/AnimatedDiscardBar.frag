#version 460
// AnimatedDiscardBar - slides a bar of discarded fragments along texture s with time
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

  fragColor.a = fract( freq * Time - uv.s * repeats ) < pw ? 1.0 : 0.4;
  if (fragColor.a < 0.5)
    discard;
} 

