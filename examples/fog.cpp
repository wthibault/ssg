// 
// fog.cpp
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
int drawMode = 0;
Material *mat = 0;

vec4  fogColor;
float fogDensity;
float fogStart = 2.0;
float fogEnd = 10;
bool  enableLayeredFog = false;

vec4 backgroundColor ( 0.6,0.6,0.6,1.0 );

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

//
// display()
//

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  static float lastFrame = getNow();
  float now = getNow();

  // update the scene graph
  root->update(now-lastFrame);

  // update fog params
  RenderingEnvironment &env = RenderingEnvironment::getInstance();
  env.setFogDensity ( fogDensity );
  env.setFogStart ( fogStart );
  env.setLayeredFogEnabled ( enableLayeredFog );
  env.setFogTopPlane ( vec4(0,1,0,0.25) );
  env.setFogBottomPlane ( vec4(0,-1,0, -4) );
  // draw
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

void printFogParams()
{
  cout << "fog density = " <<  fogDensity << endl;
  cout << "fog start = " <<  fogStart << endl;
  cout << "fog end = " <<  fogEnd << endl;
}

void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 'f':    
    fogDensity *= 1.1; 
    printFogParams();
    glutPostRedisplay();
    break;
  case 'F':    
    fogDensity *= 0.9; 
    printFogParams();
    glutPostRedisplay();
    break;

  case 's':
    fogStart += 0.01;
    printFogParams();
    glutPostRedisplay();
    break;
  case 'S':
    fogStart -= 0.01;
    printFogParams();
    glutPostRedisplay();
    break;

  case 'l':
    enableLayeredFog = ~enableLayeredFog;
    glutPostRedisplay();
    break;

  case ' ':
    drawMode = (drawMode + 1) % 3;
    glutPostRedisplay();
    break;
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
  float size = 1.0;
  float dist = 0.0;
  instance->setMatrix ( scale ( translate ( mat4(), vec3(0,0,-dist) ),
				vec3(size,size,size)) );
  instance->addChild ( prim );


  // enable camera trackball
  camera.enableTrackball (true);


  // create a material to use
  mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.9, 0.9, 0.0, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->shadowEnable = false;
  mat->program = mat->loadShaders ( "PhongShadingShadowsFog" );

  // attach the material to the instance
  instance->setMaterial ( mat );

  // a light
  RenderingEnvironment::getInstance().addPointLight ( vec3(4,6,5), vec3(0,0,0) );
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", status);
  }

  // enable fog and initialize fog params
  RenderingEnvironment &env = RenderingEnvironment::getInstance();
  env.setFogEnabled(true);
  fogDensity = 0.5;
  //  env.setFogColor ( vec4(0.0,0.0,0.0,1) );
  env.setFogColor ( backgroundColor );
  env.setFogStart ( 2 );


  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  //  glClearColor (0.0, 0.0, 0.0, 1.0);
  glClearColor ( backgroundColor.r,
		 backgroundColor.g,
		 backgroundColor.b,
		 1.0 );
  glEnable(GL_DEPTH_TEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
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



