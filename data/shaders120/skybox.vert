#version 120
// based on http://gamedev.stackexchange.com/questions/60313/implementing-a-skybox-with-glsl-version-330
attribute  vec3 vPosition;
attribute  vec3 vNormal;

varying  vec3 eyeDirection;

void main()
{
  //  mat4 iP = inverse(Projection);
  //  mat3 iM = transpose(mat3(ModelView));
  //  vec3 unproj = (iP * vec4(vPosition,1.0)).xyz;
  //  eyeDirection = iM * unproj;
  eyeDirection = vNormal;
  gl_Position = vec4(vPosition,1.0);
}
