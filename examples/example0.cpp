//
// example0.cpp
// simple example with ssg Camera
//

#include "ssg.h"
#include <iostream>

using namespace std;
using namespace ssg;
using namespace glm;

ModelNode *root;
Primitive *prim;
Camera     camera;
int        height;

void display ()
{
  // clear the screen every frame
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // draw the scene using the camera
  camera.draw(root);

  // make the image visible
  glutSwapBuffers();

}

void timer ( int delay )
{
  // schedule a new timer event in the future
  glutTimerFunc( delay, timer, delay );
  // ask glut to call display()
  glutPostRedisplay();
}

void reshape (int w, int h)
{
  // save height for mouse handling
  height = h;

  // let the camera know about the new shape
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

  //  create a primitive.
  Primitive *prim = new Triangle;

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();

  // set the instance's matrix to the identity matrix
  instance->setMatrix ( mat4() );

  // add the primitive as a child of the instance
  instance->addChild ( prim );

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 0,0,10,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );
  // create a material
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.6, 0.6, 0.6, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 400.0;
  mat->program = mat->loadShaders ( "PhongShading" );

  // attach the material to the primitive
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // enable camera trackball
  camera.enableTrackball (true);

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


