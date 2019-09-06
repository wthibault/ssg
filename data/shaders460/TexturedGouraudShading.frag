#version 120
// TexturedGouraudShading.frag = modulate the color by the texture
uniform sampler2D Texture;

varying  vec2 uv;
varying  vec4 color;


void main() 
{ 
  gl_FragColor = color * texture2D(Texture,uv);
} 

