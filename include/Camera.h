//
// Camera.h
//
#pragma once
#include "Trackball.h"
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace ssg {

class Camera {
public:
 Camera() : trackballEnabled(false),
    trackball(320,240,240) ,
    distance(6),
    position(0,0,distance),
    lookat ( 0,0,0 ),
    up ( 0,1,0 ),
    fovy ( 65.0 ),
    wscreen ( 640 ), 
    hscreen ( 480 ),
    near ( 0.2 ),
    far ( 200.0 )
      { M = glm::translate ( glm::mat4(), glm::vec3(0,0,-distance) );}

  void enableTrackball(bool on) { trackballEnabled=on; }
  void startMouse ( float x, float y ) { trackball.startMouse(x,y); }
  void dragMouse ( float x, float y )  { trackball.dragMouse(x,y); };
  void setDistance ( float d ) { distance = d; }

  void setPosition ( glm::vec3 pos ) { position = pos; M = glm::lookAt(position,lookat,up); }
  glm::vec3 getPosition ( ) { return position; }

  void setLookat   ( glm::vec3 look ) { lookat = look; M = glm::lookAt(position,lookat,up); }
  glm::vec3 getLookat   ( ) { return lookat; }

  void setUp ( glm::vec3 _up ) { up = _up; M = glm::lookAt(position,lookat,up); }
  glm::vec3 getUp () { return up; }

  void setFov ( float afovy ) { 
    // you'd better call setupPerspective from the resize callback else aspect ratio probably wrong.
    // later, you can call setFov(fovy) to change the "lens"
    fovy = afovy; 
    setupPerspective ( wscreen, hscreen );
    //    setupPerspective ( wscreen, hscreen, 1.5 ); // XXXX HACK for triplehead+canon setup summer/15
  }
  float getFov () { return fovy; }

  void setNearFar ( float _near, float _far )  { near = _near; far = _far; }
  void getNearFar ( float &_near, float _far ) { _near = near; _far = far; }
  
  void setProjectionMatrix(glm::mat4 m) {P=m;}
  glm::mat4 getProjectionMatrix() {return P;}

  void setModelviewMatrix(glm::mat4 m)  {M=m;}
  glm::mat4 getModelviewMatrix()  {return M;}

  void setupPerspective ( int w, int h, float ratio=0.0 ) {
    hscreen = h;
    wscreen = w;

    glViewport (0, 0, (GLsizei) w, (GLsizei) h); 

    if (ratio != 0.0)

      P = glm::perspective ( fovy, ratio, near, far );

    else

      P = glm::perspective ( fovy, (GLfloat) w / (GLfloat) h, near, far );

    setupTrackball();
  }

  void setupTrackball () {
    float halfw = float(wscreen)/2.0f;
    float halfh = float(hscreen)/2.0f;
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

    //    Ptr<Material> m(scene->material);
    scene->draw( M, P, scene->material_ );
  }
  void draw ( ssg::Ptr<ssg::Instance> scene ) {
    draw (  scene.get() );
  }
  void draw ( ssg::Ptr<ssg::Primitive> scene ) {
    draw (  scene.get()  );
  }

  void setScreenSize ( int _w, int _h ) {
    hscreen = _h;
    wscreen = _w;
  }

  glm::mat4 getModelview () { return M; }
  glm::mat4 getProjection () { return P; }
  
protected:
  bool      trackballEnabled;
  Trackball trackball;
  float     distance;
  glm::vec3 position, lookat, up;
  float     fovy;
  int       wscreen, hscreen;
  float     near, far;
  glm::mat4 P;
  glm::mat4 M;
  
};
};
