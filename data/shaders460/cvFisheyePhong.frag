#version 120
// cvFisheyPhong.frag - uses OpenCV fisheye camera model 

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
  //  float ldotn = dot ( L, N );
  //  float Kd = max( ldotn, -ldotn );
  float Kd = abs(dot (L,N) );
  vec4  diffuse = Kd * DiffuseProduct;
  
  //  float Ks = pow( max(dot(N, H), -dot(N,H)), Shininess );
  float Ks = pow( abs(dot(N, H)), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  gl_FragColor = diffuse + ambient + specular;
} 

