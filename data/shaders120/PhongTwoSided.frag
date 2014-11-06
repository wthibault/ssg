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

  // two sided
  //  float ndotv = dot(N,V);
  //  if ( ndotv < 0.0 )
  //    N = -1.0 * N ;

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  //  float Kd = max( dot(L, N), 0.0 );
  // twosided
  //float Kd = max ( dot(L,N), dot(L,-N) );
  float ndotv = dot ( N,V );
  //  if ( ndotv < 0.0 )
  //      N = -1.0 * N;
  //  float Kd = max( dot(L, N), -dot(L,N) );
  float Kd = dot(L, N);
  vec4  diffuse = Kd * DiffuseProduct;
  
  float Ks = pow( max(dot(N, H), 0.0), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  //  if( dot(L, N) < 0.0 ) {
  //      specular = vec4(0.0, 0.0, 0.0, 1.0);
  //    } 

  // vec4 finalColor = clamp ( ambient + diffuse + specular, 0.0, 1.0);
  // const float LOG2 = 1.442695;
  // float z = gl_FragCoord.z / gl_FragCoord.w;
  // float fogFactor = exp2( -gl_Fog.density * 
  // 				   gl_Fog.density * 
  // 				   z * 
  // 				   z * 
  // 				   LOG2 );
  // fogFactor = clamp(fogFactor, 0.0, 1.0);

  // gl_FragColor = mix(gl_Fog.color, finalColor, fogFactor );
  
  gl_FragColor = ambient + diffuse + specular;
} 

