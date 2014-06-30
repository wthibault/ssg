// Phongshading.vert = per fragment lighting

attribute  vec3 vPosition;
attribute  vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform mat4 LightMatrix;

varying vec3 Light, View, Normal;
varying vec4 ShadowMapCoord;

void main()
{
    // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  Light = normalize( LightPosition.xyz - pos );
  View = normalize( -pos );

  // Transform vertex normal into eye coordinates
  Normal = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
  

  ShadowMapCoord = LightMatrix * vec4(vPosition.xyz,1.0);
  gl_Position = Projection * ModelView * vec4(vPosition.xyz,1.0);
  

}
