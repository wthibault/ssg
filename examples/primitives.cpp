//
// 
// shows how to subclass ssg::Primitive
//

#include "ssg.h"
#include <cmath>
using namespace glm;
using namespace ssg;

Ptr<Instance> root;
Ptr<Primitive> prim;
Camera     camera;
int width, height;


///////////////////////////////////////////////////////
// MyPrimitive is a subclass of Primitive.
// (see also the code for the Triangle and Marker in <SSG-installation>/src )
//
// The constructor should push , for each vertex of the primitive:
//  vec3 vertex coordinates onto the Primitive::points_ vector,
//  vec3 normals onto    normals_;
//  vec2 texture coordinates    texCoords_;
// The indices_ vector holds a list of indices of vertices, 
//     in the order required by the primitive type
// The primitive type is the GL_ constant for the drawing primitive to use.
//
// The constructor, after loading the above vectors,
// then calls Primitive::init(), which allocates and loads the 
// vertex array and index arrays.
//
// Once init() returns,  the application can call Primitive::draw()
// to draw the object, once a Material and its shaders have been
// created.
//
// For a dynamic object, you need to get down and dirty and
// use glBufferSubData to upload the new vertex attributes or element array.
// See the code for ParticleSystem.  You can subclass Particle system,
// and use its init() and even its draw() but you have to arrange to change 
//  the points_, normals_ , etc. vectors in your own code. see ParticleSystem::update()
// as an example.
//
// This example shows building a mesh surface defined by a function z=f(x,y)
// 
///////////////////////////////////////////////////////

class MyPrimitive : public Primitive
{
private:

  vec3 bilerp (vec3 P00, vec3 P01, vec3 P10, vec3 P11, float s, float t)
  { 
    return (1-s)*(1-t)*P00 + s*(1-t)*P10 + (1-s)*t*P01 + s*t*P11; 
  }
  
// the function to plot
  float f ( float x, float y ) 
  { 
    float r = sqrt(x*x+y*y); return 0.1 * sin(20*r)/r; 
    //return 0.1*sin(10*x) + 0.1 * sin(10*y);
  }

  vec3 dfdx (float x, float y) {
    const float h = 1e-4;
    float f1 = f ( x,   y);
    float f2 = f ( x+h, y );
    return normalize ( vec3(x+h,y,f2) - vec3(x,y,f1) );
  }

  vec3 dfdy (float x, float y) {
    const float h = 1e-4;
    float f1 = f ( x, y   );
    float f2 = f ( x, y+h );
    return normalize ( vec3(x,y+h,f2) - vec3(x,y,f1) );
  }

  int indexOf ( int row, int col, int n ) {
    return row * n + col;
  }

public:
  virtual void update(float dt) {}
  MyPrimitive()
  {
    // we'll create a mesh using GL_TRIANGLES.
    float xmin = -2;
    float xmax = 2;
    float ymin = -2;
    float ymax = 2;
    const int numDivs = 1000;
    // the PNN vecs are the corners of the mesh
    // use linear intepolation to get the xy of the mesh vertices.
    vec3 P00 (xmin,ymin,0);
    vec3 P10 (xmax,ymin,0);
    vec3 P01 (xmin,ymax,0);
    vec3 P11 (xmax,ymax,0);

    for (int row=0; row < numDivs; row++ ) {
      for (int col=0; col < numDivs; col++ ) {
        // we find the parameter-space coordinates of the vertex, 
        // and lerp the corners using them as parameter
        // we generate two additional vertices of the tristrip for each column
        // step is the spacing between rows and cols, in parameter space

        float step = 1.0/numDivs;
        float s = col * step;
        float t = row * step;
        vec3 v = bilerp (P00,P01,P10,P11, s, t);
        // set the z coord by calling the function to plot
        v.z = f ( v.x, v.y );
        // add itto the primitive
        points_.push_back ( v );
        // push on normals and tex coords as well
        vec3 n = cross ( dfdx(v.x,v.y), dfdy(v.x,v.y) );
        normals_.push_back ( normalize(n) );
        texCoords_.push_back ( vec2(s, t) );
        // save indices for two tris 
        if ( row>0 && col>0 ) {
          indices_.push_back( indexOf(row-1,col-1,numDivs) );
          indices_.push_back( indexOf(row,  col-1,numDivs) );
          indices_.push_back( indexOf(row,  col,  numDivs) );

          indices_.push_back( indexOf(row,  col  ,numDivs) );
          indices_.push_back( indexOf(row-1,col  ,numDivs) );
          indices_.push_back( indexOf(row-1,col-1,numDivs) );
        }
      }
    }

    //     //debug
    // for (int i =0;i<indices_.size();i++ ) {
    //   std::cout << indices_[i] << std::endl;
    // }
    std::cout << indices_.size()/3 << " triangles\n";

    drawingPrimitive_ = GL_TRIANGLES;
    ssg::Primitive::init();
  }
};

///////////////////////////////////////////////////////
// Cone is a simpler example: a cone
class Cone : public Primitive 
{
public:
  virtual void update(float dt) {}
  Cone ()
  {
    int numDivs = 100;
    float h = 1;    // height
    float r = 0.25; // radius of base

    drawingPrimitive_ = GL_TRIANGLES;

    // first the apex
//    points_.push_back ( vec3(0,0,h) );
//    normals_.push_back ( vec3 (0,0,1) );

    // now the circular base
    vec3 vertical = vec3 (0,0,1); // points up 
    vec3 radial; // points outward from the center of the base to the vertex
	const float PI = 3.14159265358979323846;
    float step = PI * 2.0 / numDivs;
    for (int i = 0; i < numDivs+1; i++ ) {
      float angle = i * step;
      float x = r * cos ( angle );
      float y = r * sin ( angle );
      points_.push_back ( vec3(0, 0, h) );
      points_.push_back ( vec3(x, y, 0) );
      radial = normalize( vec3(x, y, 0 ));
      vec3 n = normalize( vec3( h * radial + r * vertical ) );
      normals_.push_back ( n );
      normals_.push_back ( n );
    }
    // now the primitives, first the sides
    for (int i = 0; i < numDivs; i++ ) {
      indices_.push_back ( 2*i );
      indices_.push_back ( 2*i+1 );
      indices_.push_back ( 2*i+3 );
    }
    // now the base, make a copy of the base points to change their normals (ugly)
    int baseStartVertex = points_.size();
    for (int i = 0; i < numDivs; i++ ) {
      points_.push_back ( points_[2*i+1] );
      normals_.push_back ( vec3(0,0,-1) );
    }
    // then set the indices for triangles of the base
    for (int i = 0; i < numDivs-3; i++ ) {
      indices_.push_back ( baseStartVertex);
      indices_.push_back ( baseStartVertex+i+1 );
      indices_.push_back ( baseStartVertex+i+2 );
    }
   

    ssg::Primitive::init();
  }
};
///////////////////////////////////////////////////////
// Tetraball is a tetrahedron with Gouraud-style (shared) vertex normals.
//  it's meant as an extremely low-LOD sphere/marker, best drawn small.
// even then, it will flicker and such. diff shader needed (n.l goes negative at verts)
// an example of what not to do.
class TetraBall : public Primitive 
{
private:
  vec3 v[4];
public:
  virtual void update(float dt) {}
  TetraBall() {
    v[0] = vec3 (  1,  0, -1.0/sqrt(2.0) );
    v[1] = vec3 ( -1,  0, -1.0/sqrt(2.0) );
    v[2] = vec3 (  0,  1,  1.0/sqrt(2.0) );
    v[3] = vec3 (  0, -1,  1.0/sqrt(2.0) );

    for (int i = 0; i < 4; i++) {
      points_.push_back(v[i]);
      normals_.push_back ( normalize(v[i]));
    }

    indices_.push_back ( 0 );
    indices_.push_back ( 1 );
    indices_.push_back ( 2 );

    indices_.push_back ( 0 );
    indices_.push_back ( 2 );
    indices_.push_back ( 3 );

    indices_.push_back ( 1 );
    indices_.push_back ( 3 );
    indices_.push_back ( 2 );

    indices_.push_back ( 1 );
    indices_.push_back ( 0 );
    indices_.push_back ( 3 );

    drawingPrimitive_ = GL_TRIANGLES;

    ssg::Primitive::init();
  }
};
///////////////////////////////////////////////////////


void display ()
{
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

  case 'p':
    prim->setDrawingPrimitive ( GL_POINTS );
    break;
    
  case 'l':
    prim->setDrawingPrimitive ( GL_LINES );
    break;
    
  case 't':
    prim->setDrawingPrimitive ( GL_TRIANGLES );
    break;
    
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
  
  // create a primitive using our subclass of Primitive. 

  prim = Ptr<Primitive> ( new MyPrimitive );

  // create a root Instance to contain this primitive
  Ptr<Instance> instance ( new Instance() );
  instance->setMatrix ( mat4() );
  instance->addChild ( prim );

  // the lights are global for all objects in the scene
  RenderingEnvironment::getInstance().lightPosition = vec4 ( 0,0,10,1 );
  RenderingEnvironment::getInstance().lightColor = vec4 ( 1,1,1,1 );

  // create a material to use
  Ptr<Material> mat ( new Material );
  mat->ambient = vec4 ( 0.1, 0.1, 0.1, 1.0 );
  mat->diffuse = vec4 ( 0.8, 0.8, 0.5, 1.0 );
  mat->specular = vec4 ( 1.0, 1.0, 1.0, 1.0 );
  mat->shininess = 133.0;
  mat->program = mat->loadShaders ( "PhongShading" );

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


