// PhongShading.vert = per fragment lighting

attribute  vec3 vPosition;
attribute  vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;

varying vec3 Light, View, Normal;

void main()
{
    // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  Light = normalize( LightPosition.xyz - pos );
  View = normalize( -pos );

  // Transform vertex normal into eye coordinates
//  Normal = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
    Normal = normalize( transpose(inverse(ModelView)) * vec4(vNormal,0.0) ).xyz;
  //Normal = normalize( gl_NormalMatrix*vNormal );
  if ( dot ( Normal, View ) < 0.0 )
      Normal = -Normal;
  
  gl_Position = Projection * ModelView * vec4(vPosition.xyz,1.0);
  

}
