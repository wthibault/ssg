#version 120
//
// PhongShadingShadowsFog.frag = compute fragment lighting with shadows
//
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;
//uniform sampler2DShadow ShadowMapTexture;
uniform sampler2D ShadowMapTexture;
uniform int  ShadowEnable;
uniform float FogDensity;
uniform vec4  FogColor;
uniform float FogStart;
//uniform float FogEnd; // ignored for now
uniform int   EnableLayeredFog;
// should have unit normals, and be in eye space
// outward facing (wrt fog)
uniform vec4  FogBottomPlane;  
uniform vec4  FogTopPlane;


// these are all in eye coords
varying vec3 Light, View, Normal, EyeCoords;
varying vec4 ShadowMapCoord;

const float EPSILON = 1.0e-6;
const float BIGNUMBER = 1.0e6;

float intersectEyeRayPlane( vec3 dir, vec4 plane)
{
  // for a ray with tail at origin and direction dir
  float ndotdir = dot ( plane.xyz, dir );
  if ( abs(ndotdir) < EPSILON )
    return BIGNUMBER;
  else
    return -plane.w / ndotdir;
}

float layeredFogLength ()
{
  // eye is at the origin and so is the ray tail. length in fog?
  vec3 dir = normalize (EyeCoords);
  float tsurf = length (EyeCoords);
  float t1 = intersectEyeRayPlane ( dir, FogBottomPlane );
  float t2 = intersectEyeRayPlane ( dir, FogTopPlane );
  
  if ( t1 < 0.0 && t2 < 0.0 )
    return 0.0;
  else if ( t1 <= 0.0 && t2 > 0.0 )
    return min(tsurf, t2);
  else if ( t1 > 0.0 && t2 <= 0.0 )
    return min(tsurf, t1);
  else {
    float tmin = min(t1,t2);
    float tmax = max(t1,t2);
    if (tsurf < tmin)
      return 0.0;
    else if (tsurf < tmax)
      return tsurf - tmin;
    else
      return tmax - tmin;
  }
}



void main() 
{ 
  vec4 color;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  float Kd = max( dot(L, N), 0.0 );
  vec4  diffuse = Kd * DiffuseProduct;
  
  float visibility = 1.0;
  vec4 smCoord = ShadowMapCoord / ShadowMapCoord.w;
  if ( ShadowEnable==1 && texture2D( ShadowMapTexture, smCoord.xy ).z < smCoord.z){
    visibility = 0.0;
  }
  

  float Ks =  pow( max(dot(N, H), 0.0), Shininess );

  vec4  specular = Ks * SpecularProduct;
  
  vec4 surfaceColor = clamp ( ambient + visibility* (diffuse + specular), 0.0, 1.0);

  float z;

  if (EnableLayeredFog==1)
    z = layeredFogLength();
  else
    z = length ( EyeCoords );

  float fogDist = max ( 0.0, z - FogStart );
  fogDist *= fogDist;

  float fogFactor = exp ( -FogDensity * fogDist );
  gl_FragColor = fogFactor * surfaceColor + (1-fogFactor) * FogColor;
} 

