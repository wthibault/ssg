#version 120
// cvDistConstant.frag - uses OpenCV camera model
// use constant shading with diffuse color
//
uniform sampler2D Texture; // diffuse surface texture
uniform sampler2D DiffuseIrradiance;
uniform sampler2D SpecularIrradiance;

uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform float Shininess;

// these are all in eye coords
varying vec3 Light, View, Normal;
varying vec2 uv;


////////////////////////////

void main() 
{ 

  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);

} 

