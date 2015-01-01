//
// ptrTest.cpp
// try to test the smart pointers
//

#include "ssg.h"
using namespace glm;
using namespace ssg;

Ptr<Instance> root;
Camera        camera;

int width, height;

// for the scene graph
const int branchingFactor = 5;
const int depth = 3;


Ptr<Instance>
createGraph ( int n, int level )
{
  Ptr<Primitive> prim ( new Triangle ); // shared
  Ptr<Instance> instance ( new Instance );
  for (int i=0; i<n; i++) {
    if ( level ) {
      Ptr<Instance> child ( createGraph ( n, level-1 ) ); 
      glm::vec3 pos ( urand()-0.5, urand()-0.5, urand()-0.5 ) ;
      child->setMatrix ( glm::translate ( glm::mat4(), pos ) );
      instance->addChild ( child );
    } else {
      instance->addChild ( prim );
    }
  }
  return instance;
}

Ptr<Instance>
createSimpleGraph ()
{
  Ptr<Instance> instance ( new Instance );
  Ptr<Primitive> prim  ( new Triangle );
  instance->addChild (prim);
  return instance;
}


void display ()
{
  // clobber the old graph with a new one
  // check with top to see if memory is leaking

  root = createGraph ( branchingFactor, depth );
  //  root = createSimpleGraph();

  Ptr<Material> mat ( new Material );
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.1, 0.5, 0.5, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "BumpMappedTexturedPhongShading" );
  root->setMaterial ( mat );

  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  camera.draw(root);
  glutSwapBuffers();
}

void timer ( int delay )
{
  root->update(0.033); // assumes called at 30Hz
  glutTimerFunc( delay, timer, delay );
  glutPostRedisplay();
}

void reshape (int w, int h)
{
  width = w;
  height = h;
  camera.setupPerspective(w,h);
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


void init (int argc, char **argv)
{
  
  // allocate a scene graph

  //  Ptr<Instance> instance ( createGraph ( branchingFactor, depth ) );
  Ptr<Instance> instance ( createSimpleGraph() );

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 0,0,10,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.1, 0.5, 0.5, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "BumpMappedTexturedPhongShading" );

  // attach the material
  instance->setMaterial ( mat );

  // set the scene graph root 
  root = instance;

  // enable camera trackball
  camera.enableTrackball(true);
  camera.setDistance(3.0);

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

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
  
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc ( mouse );
  glutMotionFunc ( motion );
  glutTimerFunc(30,timer,30); 
  glutMainLoop();
  return 0;
}


