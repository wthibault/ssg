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



Ptr<Instance> root;

Camera camera;
int width, height;
int drawMode = 0;
ShadowTexture *shadowTexture;
Material *mat = 0;




float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

void
updatePointLightPosition()
{
  float now = getNow();
  float lightRotationRate = 100;
  float lightRadius = 10.0;
  float theta = 2*M_PI * now / lightRotationRate;
  float x = lightRadius * cos ( theta );
  float y = lightRadius * sin ( theta );
  float z = 5.0;
  RenderingEnvironment::getInstance().getPointLight(0).getLightCamera().setPosition(vec3(x,y,z));
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

  // recompute the shadow
  updatePointLightPosition();
  RenderingEnvironment::getInstance().getPointLight(0).updateShadow(root.get());

  if ( drawMode == 1 ) {

    // draw the view from the light
    RenderingEnvironment::getInstance().getPointLight(0).getLightCamera().draw(root);

  } else if ( drawMode == 2 ) {

    // XXX NOT WORKINGG XXX
    // draw the shadow texture
    shadowTexture->unbind(2);
    shadowTexture->bind(0);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    shadowTexture->renderFullscreenQuad();

  } else {

    // draw the scene (will use shadow if we have the right shader in place)
    camera.draw(root);

  }


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
  case 's':
    mat->shadowEnable ^= 1;
    break;
  case ' ':
    drawMode = (drawMode + 1) % 3;
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
    //    prim->setMaterial(NULL); // make room for the shadowing shader (add a way to override baked in materials???)
  } else {
    cout << "usage: " << argv[0] << " objfile\n";
    //    exit(1);
    prim = new ObjFilePrimitive ( "objfiles/bone6.obj" );
  }

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );


  // enable camera trackball
  camera.enableTrackball (true);
  camera.setDistance ( 2 );

  // the lights are global for all objects in the scene
  // XXX old deprecated XXX
  //  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  //  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );
  


  // create a material to use
  mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.9, 0.9, 0.0, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "PhongShadingShadows" );

  // attach the material to the instance
  instance->setMaterial ( mat );

  // PointLights do shadows...
  // XXX must call this AFTER compiling the shader!!! certain things in the shadow code glsl won't compile until the shader is built.
  RenderingEnvironment::getInstance().addPointLight ( vec3(4,6,5), vec3(0,0,0) );
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    printf("FB error, status: 0x%x\n", status);
  }

  // setup to draw the shadow texture
  shadowTexture = RenderingEnvironment::getInstance().getPointLight(0).getShadowTexture();
  //shadowTexture = new Texture("textures/lichen.bmp", false, false, 0 );
  shadowTexture->setupRenderFullscreenQuad("shaders120/DebugShadows.vert","shaders120/DebugShadows.frag");



  // set the instance as the scene root
  root = instance;

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
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



