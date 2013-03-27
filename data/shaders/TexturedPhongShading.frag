#version 150
// TexturedPhongShading.frag = compute fragment lighting
uniform sampler2D Texture;
uniform sampler2D BumpTexture;
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// these are all in eye coords
in vec3 Light, View, Normal;
in vec2 uv;

out vec4 fragcolor;

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
  float ldotn = dot ( L, N );
  float Kd = max( ldotn, -ldotn );
  vec4  diffuse = Kd * DiffuseProduct;
  
  float Ks = pow( max(dot(N, H), -dot(N,H)), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  //fragcolor = texture(Texture, uv) * (ambient + diffuse + specular);
  fragcolor = texture(Texture, uv) * diffuse +  (ambient + specular);
  //  fragcolor = texture(Texture, uv);
  //  fragcolor = diffuse + texture(Texture,uv) * specular;
//  float texelSize = 1.0 / 1024.0;
//  fragcolor = texture(BumpTexture,uv);
} 

