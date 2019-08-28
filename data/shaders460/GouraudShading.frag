#version 460
// GouraudShading.frag = passthrough interpolated color.
varying  vec4 color;

void main() 
{ 
  gl_FragColor = color;
} 

