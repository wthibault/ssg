#version 460
// TexturedPhongShadingshadowsfog.vert = per fragment lighting, texture, shadows and fog

attribute  vec3 vPosition;
attribute  vec3 vNormal;
attribute  vec2 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat4 LocalToWorld;
uniform vec4 LightPosition;
uniform mat4 LightMatrix;

varying vec3 Light, View, Normal, EyeCoords;
varying vec4 ShadowMapCoord;
varying vec2 uv;

void main()
{
    // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  Light = normalize( LightPosition.xyz - pos );
  View = normalize( -pos );
  EyeCoords = pos;

  // Transform vertex normal into eye coordinates
  Normal = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;

  // works for groundplane:
  ShadowMapCoord = LightMatrix * vec4(vPosition.xyz,1.0);
  //ShadowMapCoord = LightMatrix * LocalToWorld * vec4(vPosition.xyz,1.0);
  // not working!?!?!
  //ShadowMapCoord = LightMatrix * LocalToWorld * vec4(pos,1.0);

  // flip for opengl storage order bmp?
  uv = vec2(vTexCoord.x, 1.0 - vTexCoord.y);

  gl_Position = Projection * vec4(pos,1.0);
  

}
