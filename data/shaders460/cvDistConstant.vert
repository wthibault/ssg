#version 120
// cvDistConstant.vert - use opencv camera model
// 
attribute  vec3 vPosition;
attribute  vec3 vNormal;
attribute  vec2 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection; // not used!
uniform vec4 LightPosition;

uniform vec2  FocalLength;
uniform vec2  ImageCenter;
uniform float Distortion[5];

varying vec3 Light, View, Normal;
varying vec2 uv;

void main()
{
  // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  // light in eye coords:
  // Light = normalize( LightPosition.xyz - pos );
  // light in world coords;
  Light = normalize( (ModelView * LightPosition).xyz - pos );

  View = normalize( -pos );

  // Transform vertex normal into eye coordinates
  Normal = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
  
  // warp per opencv calib3d camera model with 5 distortion params (k1, k2, p1, p2, k3)
  float x = pos.x / pos.z;
  float y = pos.y / pos.z;
  float r2 = x*x + y*y;
  float xdist = x * ( 1.0 + Distortion[0]*r2 + Distortion[1]*r2*r2 + Distortion[4]*r2*r2*r2 );
  float ydist = y * ( 1.0 + Distortion[0]*r2 + Distortion[1]*r2*r2 + Distortion[4]*r2*r2*r2 );
  xdist += 2.0*Distortion[2]*x*y + Distortion[3]*(r2 + 2.0*x*x);
  ydist += Distortion[2]*(r2 + 2.0*y*y) + 2.0*Distortion[3]*x*y;
  float u = FocalLength[0] * xdist + ImageCenter[0];
  float v = FocalLength[1] * ydist + ImageCenter[1];

  //  gl_Position = vec4 ( u,v, pos.z, 1.0 );
  gl_Position = Projection * vec4 ( u, v, -length(pos), 1.0 );
  
  // Texture coords
  //uv = vTexCoord;
  // or, flip for opengl texture storage order
  uv = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
}
