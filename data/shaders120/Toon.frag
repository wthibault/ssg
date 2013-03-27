#version 120
// Toon.frag = compute fragment toon lighting
uniform sampler2D Texture;
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// these are all in eye coords
varying vec3 Light, View, Normal;
varying vec2 uv;


void main() 
{ 
  vec4 color;
  vec4 diffuse;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  // two-sided lighting
  float ldotn = dot ( L, N );
  float Kd = max( ldotn, -ldotn );

  if (Kd > 0.95)
   diffuse = vec4(1.0,1.0,1.0,1.0);
  else if (Kd > 0.5)
    diffuse = 0.6 * DiffuseProduct;
  else if (Kd > 0.25)
    diffuse = 0.4 * DiffuseProduct;
  else if (Kd > 0.12)
    diffuse = 0.2 * DiffuseProduct;
  else
    diffuse = vec4(0.0,0.0,0.0,1.0);

  float Ks = pow( max(dot(N, H), -dot(N,H)), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  //  if( dot(L, N) < 0.0 ) {
  //    specular = vec4(0.0, 0.0, 0.0, 1.0);
  //  } 

  gl_FragColor = diffuse +  (ambient + specular);
} 

