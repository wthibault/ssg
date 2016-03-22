//
// skybox.cpp
// creates skybox using shader
//

#include "ssg.h"
using namespace glm;

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;
using namespace ssg;

///////////////////////////////////////////////////////////

class Skybox
{

private:

  // a custom Primitive class to reload norner normals on demand
  struct _Quad : Primitive
  {
    virtual void update(float dt) {}

    _Quad()
    {
      points_.clear();
      points_.push_back ( vec3 ( -1,-1,0 ) );
      points_.push_back ( vec3 (  1,-1,0 ) );
      points_.push_back ( vec3 (  1, 1,0 ) );
      points_.push_back ( vec3 ( -1, 1,0 ) );
      for (int i = 0; i < 4; i++ )
	normals_.push_back ( vec3(0,0,-1) );
      indices_.push_back ( 0 );
      indices_.push_back ( 1 );
      indices_.push_back ( 3 );
      indices_.push_back ( 3 );
      indices_.push_back ( 1 );
      indices_.push_back ( 2 );
      drawingPrimitive_ = GL_TRIANGLES;
      Primitive::init();
    }

    ~_Quad()
    {
    }

    void setNormals ( vector<vec3> normals )
    {
      // load the normals_ 
      normals_.clear();
      for ( auto n : normals )
	normals_.push_back ( n );
      Primitive::init();
    }
  };
  
public:

  Skybox( string texturePath="textures/zebra.bmp",
	  string skyboxShaderName="skybox" )
  {
    // load texture
    _texture = new Texture( texturePath.c_str(), false, true );
    // load shader
    _material = Ptr<Material> ( new Material );
    _material->program = _material->loadShaders ( skyboxShaderName.c_str() );
    // create geometry
    _quad = Ptr<_Quad> ( new _Quad );
  }
  
  ~Skybox()
  {
    if (_texture)
      delete _texture;
  }

  void draw ( Camera &camera )
  {

    // find the direction vectors for the corners of the viewing frustum
    vector<vec3> worldCorners;
    vector<vec4> eyeCorners;

    eyeCorners.push_back ( vec4 ( -1,-1, -1, 1 ) );
    eyeCorners.push_back ( vec4 (  1,-1, -1, 1 ) );
    eyeCorners.push_back ( vec4 (  1, 1, -1, 1 ) );
    eyeCorners.push_back ( vec4 ( -1, 1, -1, 1 ) );

    mat4 inverseProjection = inverse(camera.getProjectionMatrix());
    mat3 inverseModelview = transpose(mat3(camera.getModelviewMatrix()));

    for ( auto c : eyeCorners ) {
      //      vec3 unprojected = (inverseProjection * c).xyz;
      vec4 unproj = inverseProjection * c;
      vec3 unprojected;
      unprojected.x = unproj.x;
      unprojected.y = unproj.y;
      unprojected.z = unproj.z;
      
      worldCorners.push_back(inverseModelview * unprojected);
    }

    // pass the directions to the primitive as normals
    _quad->setNormals(worldCorners);

    // draw a fullscreen quad with the skybox shader
    mat4 ident(1.0);
    _texture->bind(0);
    _quad->draw(ident, ident, _material);
    _texture->unbind(0);
  
  }
  
private:
  Ptr<_Quad>      _quad;
  Ptr<Material>   _material;
  Texture        *_texture; // in latitude-longitude projection, 2:1 aspect ratio
};


///////////////////////////////////////////////////////////

string objectShaderName;
string skyboxShaderName; 
string skyboxTexturePath; 

Ptr<Instance> root;
Skybox       *skybox;

Camera camera;
int width, height;

float getNow() {
  return static_cast<double>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0 ;
}

vec3 lerp (vec3 start, vec3 end, float u )
{
  return vec3 ( start*(1-u) + end*u );
}

void display ()
{
  static float lastFrame = getNow();
  float now = getNow();

  root->update(now-lastFrame);

  
  // need only clear depth since the skybox will fill the screen
  glClear ( GL_DEPTH_BUFFER_BIT );

  // the skybox uses the camera matrices but draw in canonical camera coords
  skybox->draw ( camera );

  // after drawing the skybox, clear depth so everything else will obscure it
  glClear ( GL_DEPTH_BUFFER_BIT );

  // draw the scene proper
  camera.draw ( root );

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

  skybox = new Skybox(skyboxTexturePath, skyboxShaderName);
  // load in texture image
  //  skyboxTexture = new Texture ( width, height, /*fp*/true, /*mipmaps*/true, 0/* */ );

  // create skybox shader
  // compute frustum corner directions


  //  GLuint widthUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"width");
  //  glUniform1f(widthUniformLocation, width);
  //  GLuint heightUniformLocation = glGetUniformLocation(fboTexture->drawTextureShader,"height");
  //  glUniform1f(heightUniformLocation, height);

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
  Ptr<Primitive> prim;
  if ( argc == 5 ) {
    prim = Ptr<Primitive> (new ObjFilePrimitive ( argv[1] ) );
    objectShaderName = argv[2];
    skyboxShaderName = argv[3];
    skyboxTexturePath = argv[4];
  } else {
    cout << "usage: " << argv[0] << " objfilename 3DshaderName skyboxshaderName\n skyboxTexturePath";
      //    exit(1);
    prim = Ptr<Primitive> (new ObjFilePrimitive ( "objfiles/cube.obj" ) );
    objectShaderName = "PhongShading";
    skyboxShaderName = "skybox";
    skyboxTexturePath = "textures/zebra.bmp";
  }

  // create a root Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  // enable camera trackball
  camera.enableTrackball (true);

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 4,10,5,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1, 0.1, 0.2, 1.0 );
  mat->diffuse = vec4 ( 0.5, 0.7, 0.5, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "PhongShading" );

  // attach the material to the instance
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


