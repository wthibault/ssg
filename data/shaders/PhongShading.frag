// PhongShading.frag = compute fragment lighting
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// these are all in eye coords
varying vec3 Light, View, Normal;

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
  float Kd = max( dot(L, N), -dot(L, N) );
  vec4  diffuse = Kd * DiffuseProduct;
  
  float Ks = pow( max(dot(N, H), -dot(N, H)), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
//  if( dot(L, N) < 0.0 ) {
//    specular = vec4(0.0, 0.0, 0.0, 1.0);
//  } 

  gl_FragColor = ambient + diffuse + specular;
} 

