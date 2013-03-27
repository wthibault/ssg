// 
// Trackball.h - ala Gavin Bell's version of Ken Shoemake's idea
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

class Trackball
{
public:
  Trackball ( float cx, float cy, float radius )
    : _center(cx,cy), 
      _radius ( radius ),
      _v0(),
      _qdown(),
      _qnow()
{}

  void 
  startMouse ( float x, float y ) {
    glm::vec2 start ( x, y );
    _v0 = _mouseToSphere ( (start - _center) / _radius );
    _qdown = _qnow;
  }

  glm::quat
  dragMouse ( float x, float y )
  {
    glm::vec3 _v1 = _mouseToSphere ( (glm::vec2(x,y) - _center) / _radius );
    glm::quat qdrag( glm::dot(_v0,_v1), glm::cross(_v0,_v1) );
    return _qnow = glm::normalize(glm::normalize(qdrag) * _qdown);
  }


  glm::quat getQnow () { return _qnow; }
  glm::mat4 getMat4() { return glm::mat4_cast ( _qnow ); }

private:
  glm::vec3 _mouseToSphere ( glm::vec2 s0 ) {
    // from Gavin Bell's track ball + hyperboloid code in GLUT's trackball.c
    // s0 should be normalized
    float d,t,z;
    float r = 1.0f;
    d = glm::length(s0);
    if ( d < r * 0.70710678118654752440 ) {
      z = sqrt ( r*r - d*d );
    } else {
      t = r / 1.41421356237309504880;
      z = t*t / d;
    }
    return glm::normalize ( glm::vec3 ( s0, z ) );
  }
  // screen coords
  glm::vec2 _center;
  float     _radius;
  // on unit sphere
  glm::vec3 _v0;
  // quats
  glm::quat _qdown;
  glm::quat _qnow;
};
