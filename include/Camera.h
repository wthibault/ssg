//
// Camera.h
//
#pragma once
#include "Trackball.h"
#include <cmath>

namespace ssg {

class Camera {
public:
 Camera() : trackballEnabled(false),
    trackball(320,240,240) ,
    distance(6),
    position(0,0,distance),
    lookat ( 0,0,0 ),
    up ( 0,1,0 )
      { M = glm::translate ( glm::mat4(), glm::vec3(0,0,-distance) );}

  void enableTrackball(bool on) { trackballEnabled=on; }
  void startMouse ( float x, float y ) { trackball.startMouse(x,y); }
  void dragMouse ( float x, float y )  { trackball.dragMouse(x,y); };
  void setDistance ( float d ) { distance = d; }

  void setPosition ( glm::vec3 pos ) { position = pos; }
  void setLookat   ( glm::vec3 look ) { lookat = look; }

  void setProjectionMatrix(glm::mat4 m) {P=m;}
  void setModelviewMatrix(glm::mat4 m)  {M=m;}

  void setupPerspective ( int w, int h ) {
    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
    P = glm::perspective ( 65.0f, (GLfloat) w / (GLfloat) h, 0.1f, 200.0f );
    float halfw = float(w)/2.0f;
    float halfh = float(h)/2.0f;
    trackball = Trackball ( halfw, halfh, std::min ( halfw, halfw ) );
  }

  void draw ( ssg::ModelNode *scene ) {
    if (trackballEnabled) { 
      M = glm::translate(glm::mat4(),glm::vec3(0,0,-distance)) * trackball.getMat4();
    } else {      
      M = glm::lookAt ( position, lookat, up );
    }

    // M = glm::lookAt ( position, lookat, up );
    // if (trackballEnabled) 
    //   M = M * trackball.getMat4();

    scene->draw( M, P );
  }

protected:
  bool      trackballEnabled;
  Trackball trackball;
  float     distance;
  glm::vec3 position, lookat, up;
  glm::mat4 P;
  glm::mat4 M;
  
};
};
