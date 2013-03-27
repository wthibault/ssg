#version 150
// TexturedGouraudShading.frag = modulate the color by the texture
uniform sampler2D Texture;

in  vec2 uv;
in  vec4 color;

out vec4 fragcolor;

void main() 
{ 
  fragcolor = color * texture(Texture,uv);
  //fragcolor = texture(Texture,uv);
} 

