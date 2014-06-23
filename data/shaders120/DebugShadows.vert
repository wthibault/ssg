// DebugShadows.vert

attribute  vec3 vPosition;
attribute  vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform mat4 LightMatrix;

varying vec4 ShadowMapCoord;

void main()
{
	
  ShadowMapCoord = LightMatrix * vec4(vPosition.xyz,1.0);
  gl_Position = Projection * ModelView * vec4(vPosition.xyz,1.0);

}
