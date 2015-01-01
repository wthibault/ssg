//
// example4.cpp
// animation: camera motion w/linear interpolation
//

#include "ssg.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;
using namespace ssg;

Ptr<Instance> root;

Camera camera;
int width, height;

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

vec3 lerp (vec3 start, vec3 end, float u )
{
  return vec3 ( start*(1-u) + end*u );
}

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);

  // Perform a linear interpolation of the camera position, 
  // starting and ending at the times given.
  // The starting and ending positions are given.
  // The position is clamped to the (start/end) endpoint when not moving.
  float motionStartTime = 3.0;
  float motionEndTime = 6.0;
  vec3  motionStartPosition ( 0,0,6);
  vec3  motionEndPosition ( 6,6,6 );
  float u = ((now - motionStartTime) / (motionEndTime - motionStartTime) );
  if ( u < 0.0 )
    camera.setPosition ( motionStartPosition );
  else if ( u > 1.0 )
    camera.setPosition ( motionEndPosition );
  else // if ( u >= 0.0 && u <= 1.0 )
    camera.setPosition ( lerp (motionStartPosition, motionEndPosition, u ) );
  camera.draw(root);

  glutSwapBuffers();
  lastFrame = now;

  std::cout << now << std::endl;
}

void timer ( int delay )
{
  glutTimerFunc( delay, timer, delay );
  glutPostRedisplay();
}

void reshape (int w, int h)
{
  width = w;
  height = h;
  camera.setupPerspective ( w, h );
}


void keyboard (unsigned char key, int x, int y)
{
  switch (key) {

  case 27: /* ESC */
    exit(0);
    break;
  default:
    break;
  }
}

void init (int argc, char **argv)
{
  
  //  create a primitive.  if supplied on command line, read a .obj wavefront file
  Ptr<Primitive> prim;
  if ( argc == 2 ) {
    prim = Ptr<Primitive> (new ObjFilePrimitive ( argv[1] ) );
  } else {
    cout << "usage: " << argv[0] << " objfile\n";
    //    exit(1);
    prim = Ptr<Primitive> ( new ObjFilePrimitive ( "objfiles/cube.obj" ) );
  }

  // create a root Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  // disable camera trackball else our camera positioning won't work
  camera.enableTrackball (false);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Ptr<Material> mat ( new Material );
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.5, 0.5, 0.1, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "PhongShading" );

  // attach the material to the primitive
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

}


void 
mouse ( int button, int state, int x, int y )
{
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
    camera.startMouse ( x, height-y );
  }
}

void
motion ( int x, int y ) 
{
  camera.dragMouse(x,height-y);
}


int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize (300, 300); 
  glutInitWindowPosition (100, 100);
  glutCreateWindow (argv[0]);
  
#ifndef __APPLE__
  glewInit();
#endif
  init(argc,argv);
  
  glutDisplayFunc ( display );
  glutReshapeFunc ( reshape );
  glutKeyboardFunc ( keyboard );
  glutMouseFunc ( mouse );
  glutMotionFunc ( motion );
  glutTimerFunc( 33,timer,33 ); 
  glutMainLoop();
  return 0;
}


