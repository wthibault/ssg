#version 120
// PhongShading.vert = per fragment lighting

attribute  vec3 vPosition;
attribute  vec3 vNormal;
attribute  vec2 vTexCoord;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;

varying vec3 Light, View, Normal;
varying vec2 uv;

void main()
{
    // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  Light = normalize( LightPosition.xyz - pos );
  View = normalize( -pos );

  // Transform vertex normal into eye coordinates
  Normal = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
  
  gl_Position = Projection * ModelView * vec4(vPosition.xyz,1.0);
  
  //  uv = vTexCoord;
  // flip for opengl texture storage order
  uv = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
}
