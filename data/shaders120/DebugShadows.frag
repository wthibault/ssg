//
// DebugShadows.frag = compute fragment lighting with shadows
//
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;
//uniform sampler2DShadow ShadowMapTexture;
uniform sampler2D ShadowMapTexture;

// these are all in eye coords
varying vec4 ShadowMapCoord;

void main() 
{ 
  vec4 shadowcoord = ShadowMapCoord / ShadowMapCoord.w;
  //  float depth = texture(ShadowMapTexture, ShadowMapCoord).x;
  // ?? no output!?
  float depth = texture2D(ShadowMapTexture, ShadowMapCoord.xy).x;
  depth = depth;
  gl_FragColor = vec4(depth);
} 

