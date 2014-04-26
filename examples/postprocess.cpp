//
// postprocess.cpp
// apply a postprocess image filter to 3d rendering
//

#include "ssg.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
using namespace ssg;

ModelNode *root;
Primitive *prim;
Camera camera;
int width, height;

FrameBufferObject *fbo = NULL;
Texture *fboTexture = NULL;

// efx chain
class FxChain {
public:
  FxChain( float w, float h) : _width(w), _height(h) {}
  ~FxChain() {
    for (int i = 0; i < _fxTex.size(); i++ ) {
      delete _fxTex[i];
      delete _fxFbo[i];
    }
  }
  void addEffect ( std::string name ) {
    Texture *tex = new Texture (_width,_height, true, true, 0);
    FrameBufferObject *fbo = new FrameBufferObject ( tex );
    string vname ("shaders120/fxchain.vert");
    string fname ("shaders120/"+name+".frag");
    tex->setupRenderFullscreenQuad(vname.c_str(), fname.c_str());
    GLuint widthUniformLocation = glGetUniformLocation(tex->drawTextureShader,"width");
    glUniform1f(widthUniformLocation, _width);
    GLuint heightUniformLocation = glGetUniformLocation(tex->drawTextureShader,"height");
    glUniform1f(heightUniformLocation, _height);
    _fxTex.push_back(tex);
    _fxFbo.push_back(fbo);
  }

  // call start() right before drawing
  void start ( ) {
    if (_fxFbo.size() > 0 ) {
      _fxFbo[0]->bind();
    }
  } 

  // call finish() right after drawing
  void finish() {
    _fxFbo[0]->unbind();
    int i;
    for ( int i = 1; i <= _fxTex.size(); i++ )  {
      if ( i < _fxTex.size() )
	_fxFbo[i]->bind();
      _fxTex[i-1]->bind(0);
      glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      _fxTex[i-1]->renderFullscreenQuad();
      _fxTex[i-1]->unbind(0);
      if ( i < _fxTex.size() )
	_fxFbo[i]->unbind();
    }
  }

private:
  float _width, _height;
  vector<Texture*> _fxTex;
  vector<FrameBufferObject*> _fxFbo;
};

FxChain *fxChain;

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

vec3 lerp (vec3 start, vec3 end, float u )
{
  return vec3 ( start*(1-u) + end*u );
}

void
animateCamera ( Camera &camera, float now )
{
  // Perform a linear interpolation of the camera position, 
  // starting and ending at the times given.
  // The starting and ending positions are given.
  // The position is clamped to the (start/end) endpoint when not moving.
  float motionStartTime = 1.0;
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
}

void display ()
{
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);
  //  animateCamera(camera,now);

  fxChain->start();
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  camera.draw(root);
  fxChain->finish();

#if 0
  // draw the scene to the fbo
  fbo->bind();
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  camera.draw(root);
  fbo->unbind();

  // draw a fullscreen quad with the fbo texture
  fboTexture->bind(0);
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  fboTexture->renderFullscreenQuad();
  fboTexture->unbind(0);
#endif

  glutSwapBuffers();
  lastFrame = now;

}

void timer ( int delay )
{
  glutTimerFunc( delay, timer, delay );
  glutPostRedisplay();
}

void initPostprocess();

void reshape (int w, int h)
{
  width = w;
  height = h;
  camera.setupPerspective ( w, h );
  initPostprocess();
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
initPostprocess()
{
  if ( fxChain )
    delete fxChain;

  fxChain = new FxChain(width, height);
#if 1
  fxChain->addEffect ( "edge" );
  fxChain->addEffect ( "threshold" );
  fxChain->addEffect ( "blur9x" );
  fxChain->addEffect ( "blur9y" );
  for ( int i = 0; i < 3; i++ ) {
    fxChain->addEffect ( "blur9x" );
    fxChain->addEffect ( "blur9y" );
  }
#else
  fxChain->addEffect ( "zoom1" );
#endif



#if 0
  // init fbo for offscreen rendering
  if (fboTexture)
    delete fboTexture;
  fboTexture = new Texture ( width, height, /*fp*/true, /*mipmaps*/true, 0 );

  if (fbo)
    delete fbo;
  fbo = new FrameBufferObject ( fboTexture );

  // init for postprocess render from fbo
  //  fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/blur2.frag");
  //    fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/threshold.frag");
  fboTexture->setupRenderFullscreenQuad("shaders120/blur2.vert", "shaders120/edge.frag");

  //  glUseProgram(fboTexture->drawTextureShader);
  GLuint widthUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"width");
  glUniform1f(widthUniformLocation, width);
  GLuint heightUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"height");
  glUniform1f(heightUniformLocation, height);
  //  glUseProgram(0);
#endif
}


void init (int argc, char **argv)
{
  
  //  create a primitive.  if supplied on command line, read a .obj wavefront file
  ObjFilePrimitive *prim;
  if ( argc == 2 ) {
    prim = new ObjFilePrimitive ( argv[1] );
  } else {
    cout << "usage: " << argv[0] << " objfile\n";
    exit(1);
  }

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  camera.setDistance ( 3 );
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Material *mat = new Material;
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


