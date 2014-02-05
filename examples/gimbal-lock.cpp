//
// gimbal-lock.cpp -- shows gimbal lock problem
//
//
//

#include "ssg.h"

using namespace glm;
using namespace ssg;

ModelNode *root;
Primitive *prim;
mat4 projectionMatrix;
mat4 modelviewMatrix;

int width, height;

float xrot, yrot, zrot;
const float ROT_INCR = 2.0f;

void display ()
{
  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  root->update(0.033); // assumes called at 30Hz

  // tumbling view
  mat4 Rx = rotate ( mat4(), xrot, vec3(1.0, 0.0, 0.0) );
  mat4 Rz = rotate ( mat4(), zrot, vec3(0.0, 0.0, 1.0) );
  mat4 Ry = rotate ( mat4(), yrot, vec3(0.0, 1.0, 0.0) );
  mat4 mv = modelviewMatrix * Ry * Rx * Rz;

  root->draw(mv, projectionMatrix );

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
}


Instance *
createRandomInstance ( Primitive *prim )
{
  // create a new instance to refer to the same primitive, transformed
  Instance* anotherInstance = new Instance();
  anotherInstance->addChild ( prim );
  mat4 trans = translate ( mat4(),   2.0f * vec3(urand()-0.5, urand()-0.5, urand()-0.5) );
  vec3 axis = normalize ( vec3 ( urand(), urand(), urand() ) );
  float angle = urand() * 360.0f;
  mat4 rot = rotate ( mat4(), angle, axis );
  anotherInstance->setMatrix ( rot * trans );

  // create a material to use
  Material *mat = new Material;
  mat->ambient = vec4 ( 0.1,0.1,0.1, 1.0 );
  vec3 color = normalize ( vec3(urand(),urand(), urand() ) );
  mat->diffuse = vec4 ( color, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 300.0;
  if ( urand() > 0.5 )
    mat->program = mat->loadShaders ( "PhongShading" );
  else
    mat->program = mat->loadShaders ( "Toon" );
  anotherInstance->setMaterial ( mat );

  
  return anotherInstance;
}
void keyboard (unsigned char key, int x, int y)
{
  switch (key) {
    case 'x': xrot += ROT_INCR; break;
    case 'X': xrot -= ROT_INCR; break;
    case 'y': yrot += ROT_INCR; break;
    case 'Y': yrot -= ROT_INCR; break;
    case 'z': zrot += ROT_INCR; break;
    case 'Z': zrot -= ROT_INCR; break;

    case '1': xrot = 90.0; break;
    case '2': yrot = 90.0; break;
    case '3': zrot = 90.0; break;

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

  if ( argc >= 2 ) {
    prim = new ObjFilePrimitive ( argv[1] );
  } else {
    prim = new Triangle;
    std::cout << "usage: " << argv[0] << " [objfile.obj]\n";
  }
  std::cout << "Hit 'x' or 'X' to rotate about x-axis (y,Y,z,Z), 1,2,3 for 90 degree rotate\n";

  // create a root Instance to contain this primitive
  Instance *instance = new Instance();
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

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
  glutTimerFunc(30,timer,30); 
  glutMainLoop();
  return 0;
}



