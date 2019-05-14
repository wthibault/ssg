#version 120
//
// PhongShadingShadows.frag = compute fragment lighting with shadows
//
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;
//uniform sampler2DShadow ShadowMapTexture;
uniform sampler2D ShadowMapTexture;
uniform int  ShadowEnable;

// these are all in eye coords
varying vec3 Light, View, Normal;
varying vec4 ShadowMapCoord;

void main() 
{ 
  vec4 color;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  if ( dot(N,L) < 0.0 ) {
     N = N * -1.0;
     H = H * -1.0;
  }
  //  N = float(dot(N,L)<0.0) * -1.0 * N;

  float Kd = max( dot(N,L), 0.0 );
  //float Kd = max( 0.5 + dot(L, N)/2.0, 0.0 );

  vec4  diffuse = Kd * DiffuseProduct;
  
  float visibility = 1.0;
  vec4 smCoord = ShadowMapCoord / ShadowMapCoord.w;
  //  if ( textureProj( ShadowMapTexture, ShadowMapCoord ).z < smCoord.z){
  if ( ShadowEnable==1 && texture2D( ShadowMapTexture, smCoord.xy ).z < smCoord.z){
    visibility = 0.0;
  }
  

  float Ks =  min ( 1.0, pow( max(dot(N, H), 0.0), Shininess ));

  vec4  specular = Ks * SpecularProduct;
  
  gl_FragColor = clamp ( ambient + visibility* (diffuse + specular), 0.0, 1.0);
  //      gl_FragColor.a = 1.0;
  //       gl_FragColor.r = N.x;
  //        gl_FragColor.g = N.y;
  //        gl_FragColor.b = N.z;
} 

