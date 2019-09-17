#version 460
// AnimatedRings
in  vec4 color;
in  vec2 uv;

uniform float Time;

out vec4 fragColor;

void main() 
{
  float freq = 0.5;
  float pw = 0.05;
  float dist = length ( uv - vec2(0.5,0.5) );

  fragColor = color;

  fragColor.a = fract(freq*Time - 3.0*dist) < pw ? 1.0 : 0.4;

  

} 

