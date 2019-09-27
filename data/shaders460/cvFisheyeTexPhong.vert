#version 120
// cvFisheyeTexPhong.vert - use opencv fisheye camera model

attribute  vec3 vPosition;
attribute  vec3 vNormal;
attribute  vec2 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection; // not used!
uniform vec4 LightPosition;

uniform vec2  FocalLength;
uniform vec2  ImageCenter;
uniform float Distortion[4];

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
  
  // warp per opencv fisheye camera model
  // with 4 distortion params (k1, k2, k3, k4)

  float a = pos.x / pos.z;
  float b = pos.y / pos.z;

  float r = max ( float(1e-6), sqrt ( a*a + b*b ));
  float theta = atan(r);

  float theta2 = theta*theta;
  float theta_d = theta * ( 1.0
			    + Distortion[0] * theta2
			    + Distortion[1] * theta2 * theta2
			    + Distortion[2] * theta2 * theta2 * theta2
			    + Distortion[3] * theta2 * theta2 * theta2 * theta2 );

  float x = ( theta_d / r ) * a;
  float y = ( theta_d / r ) * b;

  float u = FocalLength[0] * x + ImageCenter[0];
  float v = FocalLength[1] * y + ImageCenter[1];

  //gl_Position = vec4 ( u, v, pos.z, 1.0 );
  gl_Position = Projection * vec4 ( u, v, -length(pos), 1.0 );
  
  // Texture coords
  //uv = vTexCoord;
  // or, flip for opengl texture storage order
  uv = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
}
