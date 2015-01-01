//
// texturedtriangle.cpp
// shows applying a texture to the ssg::Triangle
//

#include "ssg.h"
#include <iostream>

using namespace std;
using namespace ssg;
using namespace glm;

Ptr<Instance> root;

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
  Ptr<Primitive> prim;
  if ( argc >= 2 ) {
    prim = Ptr<Primitive> (new ObjFilePrimitive ( argv[1] ) );
  } else {
    prim = Ptr<Primitive> ( new Triangle);
  }

  // create a root Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );

  // set the instance's matrix to the identity matrix
  instance->setMatrix ( mat4() );

  // add the primitive as a child of the instance
  instance->addChild ( prim );

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 0,0,10,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );
  // create a material
  Ptr<Material> mat ( new Material );
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.9, 0.9, 0.9, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 400.0;
  mat->diffuseTexture = new Texture ( "textures/landscape.bmp", false, true );
  mat->program = mat->loadShaders ( "TexturedPhongShading" );

  // attach the material to the primitive
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // enable camera trackball
  camera.enableTrackball (true);
  camera.setDistance ( 2.5 );

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


