//
// example3.cpp
// demonstrates particle systems and the trackball
//

#include "ssg.h"
//#include "ParticleSystem.h"
//#include "Trackball.h"
using namespace glm;
using namespace ssg;

ModelNode *root;          
Primitive *prim;          
mat4 projectionMatrix;    
mat4 modelviewMatrix;

int width, height; // window size

Trackball trackball(320,240,240);


//const char *shader = "DepthMap";
const char *shader = "ConstantShading";

void 
display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  root->update(0.033); // assumes called at 30Hz

  // mouse control 
  mat4 tball = trackball.getMat4();
  mat4 mv = modelviewMatrix * tball;

  root->draw(mv, projectionMatrix );

  glutSwapBuffers();
}

void 
timer ( int delay )
{
  glutTimerFunc( delay, timer, delay );
  glutPostRedisplay();
}

void 
reshape (int w, int h)
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

Instance *
createRandomInstanceNewSystem ()
{
  // create a new instance to refer to the same primitive, transformed
  ParticleSystem *prim = new ParticleSystem();
  
  Instance* anotherInstance = new Instance();
  anotherInstance->addChild ( prim );
  vec2 pos = vec2 ( urand() - 0.5f, urand() - 0.5f );
  float len = 2*glm::length(pos);
  anotherInstance->setMatrix ( rotate ( translate( mat4(), 2.0f * vec3 ( 0.0, pos.x, pos.y ) ),
					90.0f, vec3(0,0,1) ) );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1,0.1,0.1, 1.0 );
  vec3 rimcolor = vec3 ( 1.0, 0.0, 0.0 );
  vec3 centercolor = vec3 ( 1.0, 1.0, 0.0 );
  vec3 color = (1-len)*centercolor + len*rimcolor;
  mat->diffuse = vec4 ( color, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 300.0;
  mat->program = mat->loadShaders ( shader );
  anotherInstance->setMaterial ( mat );
  
  return anotherInstance;
}

void 
keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'a':
    dynamic_cast<Instance*>(root)->addChild ( createRandomInstanceNewSystem() );
    break;

  case 27: /* ESC */
    exit(0);
    break;
  default:
    break;
  }
}

void 
init (int argc, char **argv)
{
  
  // create a primitive.  if supplied on command line, read a .obj wavefront file

  if ( argc >= 2 ) {
    prim = new ObjFilePrimitive ( argv[1] );
  } else {
    //    prim = new Triangle;
    prim = new ParticleSystem();
    std::cout << "usage: " << argv[0] << " [objfile.obj]\n";
  }
  std::cout << "Hit 'a' to add a new particle system.\n";

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 0,0,10,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );
  // gravity center is in world coords, used by all ParticleSystem instances
  ParticleSystem::gravityCenter = vec3 ( 0,-1.9,0 );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.5, 0.5, 0.1, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;

  if (argc >= 2 )
    mat->program = mat->loadShaders ( "PhongShading" );
  else
    mat->program = mat->loadShaders ( shader );

  // attach the material to the primitive
  instance->setMaterial ( mat );

  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glPointSize(1.0);
  glLineWidth(1.0);

#if 0
  // some tests
  //create a tree and check the matrices
  Instance *top = new Instance();
  top->setMatrix ( translate ( mat4(), vec3(1,2,3) ) );

  Instance *next = new Instance();
  next->setMatrix ( translate ( mat4(), vec3 ( 1,1,1 ) ) );
  top->addChild(next);

  Instance *nother = new Instance();
  nother->setMatrix ( translate ( mat4(), vec3 ( 1,1,1 ) ) );
  top->addChild(nother);

  std::cout << "top->matrix_ \n" 
	    << top->getMatrix()
	    << std::endl
	    << "top->getWorldToLocalMatrix() \n"
	    << top->getWorldToLocalMatrix()
	    << std::endl;

  std::cout << "next->matrix_ \n" 
	    << next->getMatrix()
	    << std::endl
	    << "next->getWorldToLocalMatrix() \n"
	    << next->getWorldToLocalMatrix()
	    << std::endl;

  std::cout << "nother->matrix_ \n" 
	    << nother->getMatrix()
	    << std::endl
	    << "nother->getWorldToLocalMatrix() \n"
	    << nother->getWorldToLocalMatrix()
	    << std::endl;
		   
#endif

}

void 
mouse ( int button, int state, int x, int y )
{
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
    trackball.startMouse ( x, height-y );
  }
}

void
motion ( int x, int y ) 
{
  trackball.dragMouse(x,height-y);
}


int 
main(int argc, char** argv)
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
  
  glutMouseFunc ( mouse );
  glutMotionFunc ( motion );
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(30,timer,30); 
  glutMainLoop();
  return 0;
}


