#version 120
// cvDistTexIrr.frag - uses OpenCV camera model
// light with precomputed diffuse and specular irradiance maps
//
uniform sampler2D Texture; // diffuse surface texture
uniform sampler2D DiffuseIrradiance;
uniform sampler2D SpecularIrradiance;

uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// these are all in eye coords
varying vec3 Light, View, Normal;
varying vec2 uv;


/////////////////////////////
// lat-lon environment acess

const float Pi= 3.14159265359;
const float TWOPI = 6.28318530718;
const float EPSILON = 0.00001;

float wrap ( float val )
{
  while ( val > 1.0 ) val = val - 1.0;
  while ( val < 0.0 ) val = val + 1.0;
  return val;
}

vec2 dir2LatLon ( vec3 dir )
{
  float alpha, beta, u, v;

  if ( abs(dir.x) < EPSILON ) {
    if ( dir.z > 0.0 )
      alpha = Pi;
    else
      alpha = 0.0;
  } else {
    alpha = Pi/2.0 - atan ( -dir.z, dir.x );
  }
	
  beta = asin ( dir.y );
  u = (alpha/TWOPI) + 0.5;
  v = (beta/Pi) + 0.5;
  //  u = wrap ( u );
  //  v = wrap ( v );
  return vec2(u,1.0-v);
}

////////////////////////////

void main() 
{ 
  vec4 color;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  // two-sided lighting
  //  float ldotn = dot ( L, N );
  //  float Kd = max( ldotn, -ldotn );
  // float Kd = abs(dot (L,N) );
  float Kd = dot(L,N) / 2.0 + 0.5;
    
  //vec4  diffuse = Kd * DiffuseProduct;
  vec4 diffuse = Kd * texture2D(Texture,uv);
  
  //  float Ks = pow( max(dot(N, H), -dot(N,H)), Shininess );
  float Ks = pow( abs(dot(N, H)), Shininess );
  vec4  specular = Ks * SpecularProduct;

  vec4 diffIrr = texture2D(DiffuseIrradiance, dir2LatLon(N));
  vec4 specIrr = texture2D(SpecularIrradiance, dir2LatLon(reflect(V,N)));

  // "gamma" correct
  const float gamma = 3.0;
  diffIrr.rgb = pow ( diffIrr.rgb, vec3(gamma) );
  specIrr.rgb = pow ( specIrr.rgb, vec3(gamma) );

  gl_FragColor =  (diffuse * diffIrr)  + (specular * specIrr)  +  ambient;

} 

