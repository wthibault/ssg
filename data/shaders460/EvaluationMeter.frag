#version 460
// EvaluationMeter.frag

in  vec2 uv;

out vec4 fragColor;

void main() 
{
  float lineWidth = 0.01;
  if (uv.s < 0.5 - lineWidth)
    fragColor = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  else if (uv.s > 0.5 + lineWidth)
    fragColor = vec4 ( 0.2, 0.2, 0.2, 1.0 );
  else
    fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );

} 

