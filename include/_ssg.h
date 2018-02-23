//
// _ssg.h
//  simple scene graph
// (c) William C. Thibault , 2012, 2013
// All Rights Reversed.
//
//
#pragma once

#define GL_GLEXT_PROTOTYPES 1

#ifdef __WIN32
#include <windows.h>
#endif // __WIN32

#include <stdlib.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CheckError.h"
#include "InitShader.h"
#include "Ptr.h"
#include "Texture.h"
#include "FrameBufferObject.h"


/////////////////////////////////////////////////////////////////////////////
// Material
/////////////////////////////////////////////////////////////////////////////

namespace ssg {

struct Material
{
Material() :
  ambient (0.1, 0.1, 0.1, 1.0),
    diffuse (0.8, 0.8, 0.8, 1.0),
    specular(1.0, 1.0, 1.0, 1.0 ),
    shininess(75.0),
    program(0),
    diffuseTexture(0),
    bumpTexture(0),
    shadowEnable(1),
    count_(0)
  {}
  virtual ~Material() { glDeleteProgram ( program ); }
  // XXX change load to affect self directly (program)
  unsigned int loadShaders ( const char* basename );

  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  float shininess;
  unsigned int program;
  Texture *diffuseTexture; // XXX use Ptr here...
  Texture *bumpTexture;    // XXX use Ptr here...
  int shadowEnable;
  int count_;
};


//
// RenderingEnvironment is a singleton with the global rending params
// ...the use of singleton is really just a global, but it at least
// encapsulates the (lazy) initialization. the constructor is private
// and the instance of the singleton held in the static getInstance() method
// AND is not constructed until the first call.
// (It all needs to be made thread-safe, though. )
// Usage:  RenderingEnvironment::getInstance().lightPosition = vec4(1,1,1,1);
//
// XXX put time here? and camera?
// XXX lights should be in the graph, lighting their subgraph

 class PointLight;

class RenderingEnvironment
{
 public:
  static RenderingEnvironment &getInstance()
  {
    static RenderingEnvironment instance;
    return instance;
  }

  // XXX DEPRECATED XXX
  glm::vec4 lightPosition;
  glm::vec4 lightColor;

  int getNumPointLights () { return pointLights.size(); }
  void addPointLight ( glm::vec3 pos, glm::vec3 at );
  ssg::PointLight &getPointLight(int i) { return *(pointLights[i]); }

  void setFogEnabled ( bool enabled ) { fogEnabled = enabled; }
  void setFogDensity ( float density ) { fogDensity = density; }
  void setFogColor ( glm::vec4 color ) { fogColor = color; }
  void setFogStart ( float d ) { fogStart = d; }
  void setSlabFogEnabled ( bool enabled ) { slabFogEnabled = enabled; }
  void setFogTopPlane ( glm::vec4 plane ) { fogTopPlane = plane; }
  void setFogBottomPlane ( glm::vec4 plane ) { fogBottomPlane = plane; }
  //  void setFogEnd ( float d ) { fogEnd = d; }
  glm::vec4 &getFogColor() { return fogColor; }
  float &getFogDensity() { return fogDensity; }
  bool &getFogEnabled() { return fogEnabled; }
  float &getFogStart() { return fogStart; }
  bool &getSlabFogEnabled() { return slabFogEnabled; }
  glm::vec4 &getFogBottomPlane() { return fogBottomPlane; }
  glm::vec4 &getFogTopPlane() { return fogTopPlane; }
  //  float &getFogEnd() { return fogEnd; }

 private:
 RenderingEnvironment() : 
  fogEnabled(false), 
    fogColor(0,0,0,1) ,
    fogStart ( 2.0f ),
    slabFogEnabled(false),
    fogTopPlane ( 0,1,0,-1 ),
    fogBottomPlane ( 0,-1,0,-1 )
      {};

  RenderingEnvironment(RenderingEnvironment &R); // don't implement! (its a singleton.)
  void operator=(RenderingEnvironment const &); // don't implement!

  std::vector<PointLight*>     pointLights;
  bool                         fogEnabled;
  float                        fogDensity; // XXX TODO add start+end distances
  glm::vec4                    fogColor;
  float                        fogStart;
  float                        fogEnd;
  bool                         slabFogEnabled;
  glm::vec4                    fogBottomPlane;
  glm::vec4                    fogTopPlane;
};



//////////////////////////////////////////////////////////////////
// M o d e l N o d e
// the base class for the scene graph
//////////////////////////////////////////////////////////////////

class ModelNode {
public:

 ModelNode() : parent_(), material_(), isVisible_(true), count_(0) {}
  virtual ~ModelNode() {// std::cout << "~ModelNode\n"; 
  }
  virtual void init() {}
  virtual void update( float dt ) {}
  virtual void draw ( glm::mat4 modelview,
		      glm::mat4 projection,
		      Ptr<Material> material ) {}
  virtual void setMaterial ( Ptr<Material> m );

  ModelNode*     parent_;
  Ptr<Material>  material_;
  bool      isVisible_;
  virtual glm::mat4 getWorldToLocalMatrix() {return glm::mat4();}  // XXX BORKED
  // protected:
  int       count_;  // reference count
};

//////////////////////////////////////////////////////////////////
// P r i m i t i v e
// a leaf node, holds geometry
//////////////////////////////////////////////////////////////////

class Primitive : public ModelNode {
 public:
 Primitive() : vao_(0), arrayBuffer_(0), elementBuffer_(0) {};
  virtual ~Primitive();
  friend class Ptr<Primitive>;

  virtual void init ();
  virtual void update ( float dt ) = 0;
  virtual void draw ( glm::mat4 modelview,
		      glm::mat4 projection,
		      Ptr<Material> material );

  virtual glm::mat4 getWorldToLocalMatrix();
  virtual void      setDrawingPrimitive ( GLuint prim );

  virtual void setupShader ( glm::mat4 mv, glm::mat4 proj, Ptr<Material> m );
  virtual void endShader ();
  virtual void generateAndLoadArrayBuffer();

protected:
  GLuint drawingPrimitive_;
  GLuint vao_;
  GLuint arrayBuffer_;
  GLuint elementBuffer_;
  std::vector<glm::vec3>     points_;
  std::vector<glm::vec3>     normals_;
  std::vector<unsigned int>  indices_;
  std::vector<glm::vec2>     texCoords_;
  std::vector<glm::vec3>     colors_;
  void   deleteGLBuffers_();
};

//////////////////////////////////////////////////////////////////
// I n s t a n c e
// has a matrix and a list of children
//////////////////////////////////////////////////////////////////

class Instance : public ModelNode {
public:
  Instance () : //material_(0),
    matrix_(glm::mat4(1.0))
    {};
  virtual ~Instance();
  friend class Ptr<Instance>;
  
  virtual void update ( float dt );
  virtual void draw ( glm::mat4 modelview,
		      glm::mat4 projection,
		      Ptr<Material> material) ;
  virtual void setMatrix ( glm::mat4 mat );
  virtual glm::mat4 getMatrix ();

  virtual void addChild ( ModelNode * child );
  //virtual void addChild ( Instance* child );
  //virtual void addChild ( Primitive* child );
  virtual void addChild ( Ptr<ModelNode> child );
  //virtual void addChild ( Ptr<Instance> child );
  //virtual void addChild ( Ptr<Primitive> child );

  virtual Ptr<ModelNode> getChild ( int i );

  virtual int   numChildren();

  virtual void  setChild ( ModelNode * child, int slot );
  virtual void  setChild ( Ptr<ModelNode> child, int slot );

  virtual glm::mat4 getWorldToLocalMatrix();

protected:
  std::vector<Ptr<ModelNode> > children_;
  glm::mat4 matrix_;
  glm::mat4 worldToLocal_;
  glm::mat4 localToWorld_;
};

//
//////////////////////////////////////////////////////////////////
// a Triangle primitive class
//////////////////////////////////////////////////////////////////
//

class Triangle : public Primitive {
public:
  Triangle ();
  virtual void update(float dt) {}
};


//
//////////////////////////////////////////////////////////////////
// a Marker primitive class
//////////////////////////////////////////////////////////////////
//

class Marker : public Primitive {
public:
  Marker ();
  virtual void update(float dt) {}
};



//
//////////////////////////////////////////////////////////////////
// a Quad primitive class
//////////////////////////////////////////////////////////////////
//

class Quad : public Primitive {
public:
  Quad ();
  virtual void update(float dt) {}
};

//
//////////////////////////////////////////////////////////////////
// a Quad primitive class
//////////////////////////////////////////////////////////////////
//

class LineSegment : public Primitive 
{
public:
  LineSegment ( glm::vec3 a, glm::vec3 b )
  {
    points_.push_back ( a );
    points_.push_back ( b );

    normals_.push_back ( glm::vec3 ( 0,0,1 ) );
    normals_.push_back ( glm::vec3 ( 0,0,1 ) );

    indices_.push_back ( 0 );
    indices_.push_back ( 1 );

    texCoords_.push_back ( glm::vec2 (0.0, 0.0) );
    texCoords_.push_back ( glm::vec2 (1.0, 0.0) );

    drawingPrimitive_ = GL_LINES;

    ssg::Primitive::init();
  }

  virtual void update ( float dt ) {}
};



//
//////////////////////////////////////////////////////////////////
// ObjFilePrimitive
//////////////////////////////////////////////////////////////////
//

class ObjFilePrimitive : public Primitive {
 public:
  ObjFilePrimitive ( const char *filename );
  virtual void update(float dt) {}
  virtual void draw ( glm::mat4 modelview,
		      glm::mat4 projection,
		      Ptr<Material> material ) ;
  std::vector<Ptr<Material> >         materials_;
  std::vector< std::vector<unsigned int> > groupIndices_;
  std::vector<unsigned int> groupElementArrayIds_;
};


//////////////////////////////////////////////////////////////////
// utilities
//////////////////////////////////////////////////////////////////

float urand();

std::string ssgPrependDataPath ( const char* basename );

};

std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec);
std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec);
std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat);





