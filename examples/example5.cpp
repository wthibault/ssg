//
// example5.cpp
// loads a sequence of objfiles with baked animations 
// (created with Blender's obj exporter with "Animations" checked)
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
mat4 projectionMatrix;
mat4 modelviewMatrix;

int width, height;

Trackball trackball(320,240,240);

int startFrame, endFrame;
int currentFrame;

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  root->update(0.033); // assumes called at 30Hz

  mat4 tball = trackball.getMat4();
  mat4 mv = modelviewMatrix * tball;

  if (currentFrame == endFrame) {
    currentFrame = startFrame;
  } else {
    currentFrame++;
  }

  Ptr<Material> mat;
  //  dynamic_cast<Instance*>(root)->getChild ( currentFrame-startFrame )->draw(mv,projectionMatrix,mat);
  root->getChild ( currentFrame-startFrame )->draw(mv,projectionMatrix,mat);

  glutSwapBuffers();
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
  glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
  projectionMatrix = perspective ( 65.0f, (GLfloat) w / (GLfloat) h, 1.0f, 100.0f );
  modelviewMatrix = translate ( mat4(), vec3(0,0,-4) );
  float halfw = float(w)/2.0f;
  float halfh = float(h)/2.0f;
  trackball = Trackball ( halfw, halfh, fmin ( halfw, halfw ) );
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
  
  // create a primitive.  if supplied on command line, read a .obj wavefront file
  string basename;
  if ( argc == 4 ) {
    startFrame = atoi ( argv[1] );
    endFrame = atoi ( argv[2] );
    basename = argv[3];
  } else {
    cout << "usage: " << argv[0] << " start_fram end_frame base_name\n";
    cout << " like: " << argv[0] << " 1 150 sticks_\n";
    exit(1);
  }

  // create a root Instance to contain this primitive
  Ptr<Instance> instance (new Instance());
  instance->setMatrix ( mat4() );

  // read in each frame as a child
  for (int i = startFrame; i <= endFrame; i++ ) {
    ostringstream oss(ostringstream::out);
    oss << basename << setw(6) << setfill('0') << i << ".obj";
    ObjFilePrimitive *p = new ObjFilePrimitive ( oss.str().c_str() );
    instance->addChild ( p );
  }

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
    trackball.startMouse ( x, y );
  }
}

void
motion ( int x, int y ) 
{
  trackball.dragMouse(x,y);
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


