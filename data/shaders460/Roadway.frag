#version 120
//
// PhongShadingShadowsFog.frag = compute fragment lighting with shadows
//
uniform float     Speed; // XX NOT USED!!! wtf?
uniform float     Amplitude;
uniform float     Frequency;
uniform sampler2D Texture;
uniform float     Time;

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
uniform int   EnableSlabFog;


//
// these are all in eye coords
//
varying vec3 Light, View, Normal, EyeCoords;
varying vec4 ShadowMapCoord;
// should have unit normals, and be in eye space
// outward facing (wrt fog)
uniform vec4  FogBottomPlane;  
uniform vec4  FogTopPlane;
varying vec2 uv;

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

float slabFogLength ()
{
  // eye is at the origin and so is the ray tail. find length of ray in fog
  float tsurf = length (EyeCoords);
  vec3 dir = normalize (EyeCoords);
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

  float visibility = 1.0;
  //  vec4 smCoord = ShadowMapCoord / ShadowMapCoord.w;
  //  if ( ShadowEnable==1 && texture2D( ShadowMapTexture, smCoord.xy ).z < smCoord.z )    {
  //    visibility = 0.0;
  //  }

  vec2 src;
  float speed = 0.3;
  float amp = 0.1;
  float freq = 0.25;
  src.x = fract(uv.x - speed * Time);
  src.y = fract(uv.y + amp * sin ( freq * Time ) + 0.05 * sin ( 3.14159*2.0 * src.x ) );
  
  //  // hack for border bug?
  //  src.x = src.x * 0.9 + 0.01;
  //  src.y = src.y * 0.9 + 0.01;
  

  //  vec4  surfaceColor = clamp ( ambient + visibility* (DiffuseProduct), 0.0, 1.0);
  vec4  surfaceColor = clamp ( visibility* texture2D(Texture,src), 0.0, 1.0 );
  surfaceColor.a = 1.0;  
  float z = length ( EyeCoords );
  z = z * z;
  float fogDist = max ( 0.0, z - FogStart );
  float fogFactor = exp ( -FogDensity * fogDist );
  gl_FragColor = fogFactor * surfaceColor + (1-fogFactor) * FogColor;

  //  gl_FragColor = texture2D(Texture,src);
} 

