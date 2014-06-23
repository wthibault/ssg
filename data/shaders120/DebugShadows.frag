//
// DebugShadows.frag = compute fragment lighting with shadows
//
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;
uniform sampler2DShadow ShadowMapTexture;

// these are all in eye coords
varying vec4 ShadowMapCoord;

void main() 
{ 
  float depth = texture(ShadowMapTexture, ShadowMapCoord).x;
  depth = 1.0 - (1.0 - depth) * 25.0;
  gl_FragColor = vec4(depth);
} 

