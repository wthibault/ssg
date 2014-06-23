// 
// shadows.cpp
//

#include "ssg.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>


using namespace ssg;
using namespace glm;
using namespace std;



ModelNode *root;
Primitive *prim;
Camera camera;
int width, height;
int mainWindow, shadowMapWindow;

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

  // update the scene graph
  root->update(now-lastFrame);

  // recompute the shadow
  RenderingEnvironment::getInstance().getPointLight(0).updateShadow(root);

  // draw the view from the light
  glutSetWindow ( shadowMapWindow );
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  RenderingEnvironment::getInstance().getPointLight(0).getLightCamera().draw(root);
  glutSetWindow ( mainWindow );

  // draw the scene (will use shadow if we have the right shader in place)
  camera.draw(root);

  glutSwapBuffers();
  lastFrame = now;

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
  ObjFilePrimitive *prim;
  if ( argc == 2 ) {
    prim = new ObjFilePrimitive ( argv[1] );
    prim->setMaterial(NULL); // make room for the shadowing shader (add a way to override baked in materials???)
  } else {
    cout << "usage: " << argv[0] << " objfile\n";
    exit(1);
  }

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );


  // enable camera trackball
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  // XXX old deprecated XXX
  //  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  //  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );
  
  // PointLights do shadows...
  RenderingEnvironment::getInstance().addPointLight ( vec3(4,6,5), vec3(0,0,0) );

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", status);
  }



  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.9, 0.0, 0.0, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "PhongShadingShadows" );

  // attach the material to the instance
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

  //
  // window to display the shadow map texture
  //
  mainWindow = glutGetWindow();
  shadowMapWindow = glutCreateWindow("shadow map");
  
  glutSetWindow(mainWindow);
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



