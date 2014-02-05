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
  vec4 diffuse = DiffuseProduct;
  
  vec4 finalColor = clamp ( ambient+diffuse, 0.0, 1.0);
  const float LOG2 = 1.442695;
  float z = gl_FragCoord.z / gl_FragCoord.w;
  //    float z = -View.z;
  // exponential
      float fogFactor = exp2( -gl_Fog.density * 
    				   gl_Fog.density * 
    				   z * 
    				   z * 
    				   LOG2 );
    // linear
      //  float fogFactor = (gl_Fog.end - z) / (gl_Fog.end - gl_Fog.start);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  gl_FragColor = mix(gl_Fog.color, finalColor, fogFactor );
} 

