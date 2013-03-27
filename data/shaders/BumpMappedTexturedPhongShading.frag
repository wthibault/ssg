#version 150
// TexturedPhongShading.frag = compute fragment lighting
uniform sampler2D Texture;
uniform sampler2D BumpTexture;
uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// the tangent vector SHOULD be set by application, passed in a uniform.
// but we don't want to do that for this example!!
// so we'll just assume the tangent is along the "positive x" direction in eye space
// and make a tangent vector by crossing the eye-space positive y axis with N 
// to get the eye-space tangent vector, T,
// then cross the normal and the the tangent to get the bi-tangent vector, B.
// NEXT, we'll create a matrix to transform everything into "tangent space"
// where we can use the partial derivatives of the bump map to perturb N by adding them (and renormalizing N)
// whew.

// these are all in eye coords
in vec3 Light, View, Normal;
in vec2 uv;

out vec4 fragcolor;

vec2 bumpGradientAt ( vec2 uv )
{
  float sz = 1.0 / 1024.0;
  vec2 left = vec2 ( -sz, 0.0 );
  vec2 up = vec2 ( 0.0, +sz );
  float du = texture(BumpTexture, uv+left).r - texture (BumpTexture, uv-left).r;
  float dv = texture(BumpTexture, uv+up).r - texture (BumpTexture, uv-up).r;
  return vec2 ( du/2.0, dv/2.0 );
}

void main() 
{ 
  vec4 color;
  vec3 L = normalize(Light);
  vec3 V = normalize(View);
  vec3 N = normalize(Normal);
  vec3 H = normalize ( L + V );

  // get T and B, assuming tangent vector "along eye-y but perp. to N"
  vec3 T = cross ( N, vec3(0.0, 1.0, 0.0) );
  vec3 B = cross ( N, T );

  // form the matrix to transform from eye space to tangent space
  mat3 M = transpose ( mat3 ( T, B, N ) );
  
  // take lighting vectors to tangent space
  N = M * N;
  T = M * T;
  B = M * B;
  L = M * L;
  V = M * V;
  H = M * H;

  // find the bump and add it to N
  vec2 g = bumpGradientAt ( uv );
  N = normalize ( N + vec3 ( g, 0.0 ) );

  // Now compute the lighting model:

  // Compute terms in the illumination equation
  vec4 ambient = AmbientProduct;
  
  // two-sided lighting
  float ldotn = dot ( L, N );
  float Kd = max( ldotn, -ldotn );
  vec4  diffuse = Kd * DiffuseProduct;
  
  float Ks = pow( max(dot(N, H), -dot(N,H)), Shininess );
  vec4  specular = Ks * SpecularProduct;
  
  fragcolor = texture(Texture, uv) * diffuse +  (ambient + specular);
  //  fragcolor =  diffuse + ambient + specular;
} 

