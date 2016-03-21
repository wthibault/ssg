#version 120
// based on http://gamedev.stackexchange.com/questions/60313/implementing-a-skybox-with-glsl-version-330
// we want to do a texture lookup. for now use xyz for color

uniform sampler2D Texture;
varying vec3 eyeDirection;

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
  u = wrap ( u );
  v = wrap ( v );
  return vec2(u,v);
}

void main()
{
  //  gl_FragColor = vec4 (eyeDirection, 1.0);
  gl_FragColor = vec4 (texture2D(Texture,dir2LatLon(eyeDirection)));
}
