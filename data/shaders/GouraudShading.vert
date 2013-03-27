#version 150
// GouraudShading.vert = pervertex lighting

attribute  vec3 vPosition;
attribute  vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

varying vec4 color;


void main()
{
    // Transform vertex  position into eye coordinates
  vec3 pos = (ModelView * vec4(vPosition.xyz,1.0)).xyz;
	
  vec3 L = normalize( LightPosition.xyz - pos );
  vec3 V = normalize( -pos );
  vec3 H = normalize( L + V );

  // Transform vertex normal into eye coordinates
  vec3 N = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
  
  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  float Kd = max( dot(L, N), 0.0 );
  vec4  diffuse = Kd*DiffuseProduct;
  
  float Ks = pow( max(dot(N, H), 0.0), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  if( dot(L, N) < 0.0 ) {
    specular = vec4(0.0, 0.0, 0.0, 1.0);
  } 
  
  gl_Position = Projection * ModelView * vec4(vPosition.xyz,1.0);
  
  color = ambient + diffuse + specular;
  color.a = 1.0;


}
