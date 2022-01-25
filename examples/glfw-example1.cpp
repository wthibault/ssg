//
// glfw-example1.cpp
// a simple ssg program to display a .obj file with GLFW
//

//#define GLFW_INCLUDE_NONE
//#define GL_GLEXT_PROTOTYPES 1

//#include <GL/glut.h>

#define __USING_GLFW__ 1
#include "ssg.h"
#include <GLFW/glfw3.h>

using namespace glm;
using namespace ssg;

Ptr<Instance> root;
Camera        camera;

int width, height;
bool dragging = false;


// void display ()
// {
//   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//   camera.draw(root);
//   glutSwapBuffers();
// }

// void timer ( int delay )
// {
//   root->update(0.033); // assumes called at 30Hz
//   glutTimerFunc( delay, timer, delay );
//   glutPostRedisplay();
// }

// void reshape (int w, int h)
// {
//   width = w;
//   height = h;
//   camera.setupPerspective(w,h);
// }

// void keyboard (unsigned char key, int x, int y)
// {
//   switch (key) {

//   case 27: /* ESC */
//     exit(0);
//     break;
//   default:
//     break;
//   }
// }


//
// S S G
//

void initSSG (int argc, char **argv)
{
  //  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if ( err != GLEW_OK ) {
    std::cout << "glew init failed" << std::endl;
    exit(1);
  }
  
  // create a primitive.  if supplied on command line, read a .obj wavefront file

  Ptr<Primitive> prim;
  if ( argc >= 2 ) {
    prim = Ptr<Primitive> (new ObjFilePrimitive ( argv[1] ));
  } else {
    prim = Ptr<Primitive> (new Triangle);
    std::cout << "usage: " << argv[0] << " [objfile.obj]\n";
  }

  // create an Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

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

  camera.setupPerspective ( width, height );

  // misc OpenGL state
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);

}

//
// GLFW
//
GLFWwindow* window;

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos )
{
  if (dragging) {
    camera.dragMouse( xpos, height - ypos );
  }
}

static void key_callback ( GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key==GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose ( window, GLFW_TRUE );
  }
}

void updateCamera() {



  // mouse polling for buttons
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
  if (state == GLFW_PRESS && !dragging ) {
    double x, y;
    glfwGetCursorPos ( window, &x, &y );
    camera.startMouse ( x, height - y );
    dragging = true;
    std::cout << "start drag" << std::endl;
  }
  if (state == GLFW_RELEASE && dragging) {
    dragging = false;
    std::cout << "end drag" << std::endl;
  }
}

void framebuffer_size_callback ( GLFWwindow *window, int width, int height )
{
  camera.setupPerspective ( width, height );
}

void init(int argc, char** argv)
{
  if (!glfwInit())
    return;


  // create an OpenGL window
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  
  window = glfwCreateWindow(640, 480, "glfw-example", NULL, NULL);
  if (!window) {
    // Window or OpenGL context creation failed
    std::cout << "error making glfw window" << std::endl;
    return;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwGetWindowSize ( window, &width, &height );
  

  // register callbacks
  glfwSetErrorCallback(error_callback);
  glfwSetCursorPosCallback ( window, cursor_position_callback );
  glfwSetKeyCallback ( window, key_callback );
  glfwSetFramebufferSizeCallback ( window, framebuffer_size_callback );

  // init various SSG and rendering settings
  initSSG(argc,argv);

}

void shutdown()
{
  if (window)
    glfwDestroyWindow(window);
  glfwTerminate();
}


int main(int argc, char** argv)
{
  init(argc,argv);

  while (!glfwWindowShouldClose(window)) {

    updateCamera();
      
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    camera.draw(root);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  shutdown();

  return 0;
}  
// void 
// mouse ( int button, int state, int x, int y )
// {
//   if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
//     camera.startMouse ( x, height-y );
//   }
// }

// void
// motion ( int x, int y ) 
// {
//   camera.dragMouse(x,height-y);
// }
//{
//   glutInit(&argc, argv);
//   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
//   glutInitWindowSize (300, 300); 
//   glutInitWindowPosition (100, 100);
//   glutCreateWindow (argv[0]);
  
// #ifndef __APPLE__
//   glewInit();
// #endif
//   init(argc,argv);
  
//   glutDisplayFunc(display);
//   glutReshapeFunc(reshape);
//   glutKeyboardFunc(keyboard);
//   glutMouseFunc ( mouse );
//   glutMotionFunc ( motion );
//   glutTimerFunc(30,timer,30); 
//   glutMainLoop();
//   return 0;
//}


