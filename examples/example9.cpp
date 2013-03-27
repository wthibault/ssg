//
// example9.cpp
// articulated figure
//

#include "ssg.h"
#include "Camera.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

ModelNode *root;
Primitive *prim;
Camera camera;
int width, height;
vector<vec3> controlPoints;
Instance *link0, *link1, *link2;

vec3 upperArmJointOffset (1,1,0);
vec3 lowerArmJointOffset (1,0,0);

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

///////////////////////////////////////////////////////////////

mat4
upperArmMatrix ( float angle )
{
  mat4 t = translate ( mat4(), vec3 ( upperArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}

mat4
lowerArmMatrix ( float angle )
{
  mat4 t = translate ( mat4(), vec3 ( lowerArmJointOffset ) );
  mat4 r = rotate ( mat4(), angle, vec3(0,0,1) );
  return t * r;
}

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);

  link1->setMatrix ( upperArmMatrix ( 60 * sin(now) ) );
  link2->setMatrix ( lowerArmMatrix ( 45 * sin(now) ) );

  camera.draw(root);

  glutSwapBuffers();
  lastFrame = now;

  //  std::cout << now << std::endl;
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
  ObjFilePrimitive *body, *upperArm, *lowerArm;
  body = new ObjFilePrimitive ( "objfiles/body.obj" );
  upperArm = new ObjFilePrimitive ( "objfiles/upper_arm.obj" );
  lowerArm = new ObjFilePrimitive ( "objfiles/lower_arm.obj" );

  // create the graph

  link0 = new Instance;
  link0->addChild ( body );
  link0->setMatrix ( mat4() );

  link1 = new Instance;
  link1->addChild ( upperArm );
  link1->setMatrix ( upperArmMatrix(0) );
  link0->addChild ( link1 );

  link2 = new Instance;
  link2->addChild ( lowerArm );
  link2->setMatrix ( lowerArmMatrix(0) );
  link1->addChild ( link2 );



  // set the instance as the scene root
  root = link0;

  // enable camera trackball
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

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


