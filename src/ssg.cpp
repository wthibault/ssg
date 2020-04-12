// 
// ssg.cpp
//  simple scene graph
// (c) William C. Thibault, 2012, 2013, 2014
// All Rights Reversed.
//
//
#define GLM_SWIZZLE
#include "ssg.h"
#include <fstream>


// XXX CHANGE THIS PER YOUR opengl's glsl version
std::string shaderDir ("shaders460"); // #version 120 (mac)


#ifdef USE_APPLE_VERTEXARRAYS
#define   glGenVertexArrays glGenVertexArraysAPPLE
#define   glBindVertexArray glBindVertexArrayAPPLE
#define   glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif


/////////////////////////////////////////////////////////////////
// ModelNode
/////////////////////////////////////////////////////////////////

void
ssg::ModelNode::setMaterial ( Ptr<Material> m ) { 
  material_ = m; 
};

//ssg::ModelNode* ssg::ModelNode::top;

//////////////////////////////////////////////////////////////////
// Primitive
//////////////////////////////////////////////////////////////////
ssg::Primitive::~Primitive()
{
  //  std::cout << "~Primitive\n";
  //  if ( material_ )
  //    delete material_;
  deleteGLBuffers_();

  points_.clear();
  normals_.clear();
  indices_.clear();
  texCoords_.clear();
  colors_.clear();
}

void
ssg::Primitive::print ( int indent )
{
  for (int i=0;i<indent;i++)
    std::cout << "| ";
  std::cout << name_ << " Primitive " << std::endl;
}

void
ssg::Primitive::deleteGLBuffers_()
{
  if ( arrayBuffer_ ) glDeleteBuffers ( 1, &arrayBuffer_);
  if ( elementBuffer_ ) glDeleteBuffers ( 1, &elementBuffer_);
  if ( vao_ ) glDeleteVertexArrays ( 1, &vao_);
}

void
ssg::Primitive::generateAndLoadArrayBuffer()
{
  deleteGLBuffers_();

  glGenVertexArrays( 1, &vao_ );
  glBindVertexArray( vao_ );

  //  GLuint buffer;
  // Create and initialize a buffer object
  glGenBuffers( 1, &arrayBuffer_ );
  glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer_ );

  //
  //  use attributes for position (vec3)
  //   and normals(vec3),
  //   AND texCoords (vec2). 
  //  each primitive will be drawn with a single material, 
  //  "light products" set via shader uniforms.
  //   
  //

  // compute the memory needs for points normals and indicies
  long int sizeofPoints = sizeof(glm::vec3)*points_.size();
  int sizeofNormals = sizeof(glm::vec3)*normals_.size();
  int sizeofTexCoords = sizeof(glm::vec2)*texCoords_.size();

  glBufferData( GL_ARRAY_BUFFER, 
		sizeofPoints + sizeofNormals + sizeofTexCoords,
		NULL, GL_DYNAMIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeofPoints, &points_[0] );
  glBufferSubData( GL_ARRAY_BUFFER, sizeofPoints, sizeofNormals, &normals_[0] );
  if ( sizeofTexCoords )
	glBufferSubData( GL_ARRAY_BUFFER, sizeofPoints + sizeofNormals, sizeofTexCoords, &texCoords_[0] );
  // unbind the the arraybuffer, but keep the vertex array bound
  glBindBuffer ( GL_ARRAY_BUFFER, 0 );
}


void 
ssg::Primitive::init () {

  // call this after setting up the points and colors vector in the derived class
  // Create a vertex array object

  generateAndLoadArrayBuffer();

  // setup the element array for the triangle indices
  //  GLuint indexBufferId;
  glGenBuffers(1, &elementBuffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer_);
  int sizeofIndices = indices_.size()*sizeof(unsigned int);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofIndices, &(indices_[0]), GL_DYNAMIC_DRAW);


  glBindVertexArray ( 0 );
}


glm::vec4 transformPlane ( glm::vec4 &plane, glm::mat4 &M )
{
  // based on http://stackoverflow.com/questions/7685495/transforming-a-3d-plane-by-4x4-matrix
  // needs "#define GLM_SWIZZLE" before including glm.hpp
  glm::vec3 pn (plane.xyz);
  glm::vec4 P = glm::vec4 ( pn * plane.w, 1.0 );
  glm::vec4 N = glm::vec4 ( pn, 0.0 );
  P = M * P;
  N = glm::transpose ( glm::inverse ( M ) ) * N;
  float d = glm::dot ( glm::vec3(P.xyz), glm::vec3(N.xyz) );
  return glm::vec4 ( N.xyz, d );
}

void
ssg::Primitive::setDrawingPrimitive ( GLuint prim )
{
  drawingPrimitive_ = prim;
}

void
ssg::Primitive::setupShader ( glm::mat4 modelview,
			      glm::mat4 projection,
			      Ptr<Material> material )
{

  if ( !material.get() ) {
    // no material passed in
    if ( !material_.get() ) {
      // no material on primitive, make one now and keep it
      material_ = Ptr<Material> (new Material);
      material_->ambient = glm::vec4 ( 0.2, 0.2, 0.2, 1.0 );
      material_->diffuse = glm::vec4 ( 0.7, 0.7, 0.7, 1.0 );
      material_->specular = glm::vec4 ( 0.1, 0.1, 0.1, 1.0 );
      material_->shininess = 200.0;
      //      material_->program = material_->loadShaders ( "BumpMappedTexturedPhongShading" );
      material_->program = material_->loadShaders ( "PhongTwoSided" );
      // XXX default textures
      material = material_;
    } else {
      material = material_;  // use the one we have (none given)
    }
  }

  // use the shader
  glUseProgram ( material->program );

  // bind
  glBindVertexArray ( vao_ );
  glBindBuffer( GL_ARRAY_BUFFER, arrayBuffer_ );

  // set up vertex arrays (to vao's vertex array buffer)
  long int sizeofPoints = sizeof(glm::vec3)*points_.size();
  int sizeofNormals = sizeof(glm::vec3)*normals_.size();
  int sizeofTexCoords = sizeof(glm::vec2)*texCoords_.size();

  GLuint vPosition = glGetAttribLocation( material->program, "vPosition" );
  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0, 
			 (GLvoid *)(0));

  GLuint vNormal = glGetAttribLocation( material->program, "vNormal" ); 
  glEnableVertexAttribArray( vNormal );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			 (GLvoid *)sizeofPoints );

  GLuint vTexCoords = glGetAttribLocation ( material->program, "vTexCoord" );
  glEnableVertexAttribArray ( vTexCoords );
  glVertexAttribPointer ( vTexCoords, 2, GL_FLOAT, GL_FALSE, 0 ,
			  (GLvoid *)(sizeofPoints + sizeofNormals) );


  // load up the matrix uniforms
  glUniformMatrix4fv( glGetUniformLocation ( material->program, "ModelView" ),
		      1, GL_FALSE, glm::value_ptr(modelview));
  glUniformMatrix4fv( glGetUniformLocation ( material->program, "Projection" ),
		      1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv ( glGetUniformLocation ( material->program, "LocalToWorld" ),
		       1, GL_FALSE, glm::value_ptr( localToWorld_ ) );

    
  // load the lighting model uniforms

  glUniform4fv ( glGetUniformLocation ( material->program, "AmbientProduct"),
		 1, 
		 glm::value_ptr ( material->ambient ) );

  // compute the lighting products, which are the reflected color for a particular light source
  // element-wise products of the light color and the material color 
  glm::vec4 diffuseProduct = 
    RenderingEnvironment::getInstance().lightColor * material->diffuse; 
  glUniform4fv (  glGetUniformLocation ( material->program, "DiffuseProduct"),
		 1,
		 glm::value_ptr ( diffuseProduct ) );

  glm::vec4 specularProduct = 
    RenderingEnvironment::getInstance().lightColor * material->specular; 
  glUniform4fv ( glGetUniformLocation ( material->program, "SpecularProduct"),
		 1,
		 glm::value_ptr ( specularProduct ) );

  glUniform1f ( glGetUniformLocation ( material->program, "Shininess" ),
		material->shininess );

  // textures
  if ( material->diffuseTexture.get() ) {
    material->diffuseTexture->bind(0);
    glUniform1i ( glGetUniformLocation ( material->program, "Texture" ),
		  0 );
  }

  if ( material->bumpTexture.get() ) {
    material->bumpTexture->bind(1);
    glUniform1i ( glGetUniformLocation ( material->program, "BumpTexture" ), 
		  1 );
  }

  if ( material->diffuseIrradianceTexture.get() ) {
    material->diffuseIrradianceTexture->bind(2);
    glUniform1i ( glGetUniformLocation ( material->program, "DiffuseIrradiance" ), 
		  2 );
  }

  if ( material->specularIrradianceTexture.get() ) {
    material->specularIrradianceTexture->bind(3);
    glUniform1i ( glGetUniformLocation ( material->program, "SpecularIrradiance" ), 
		  3 );
  }

  

  // load the light info from the rendering environment (same for all prims)
  // XXX take this to world coords 

  if (RenderingEnvironment::getInstance().getNumPointLights() == 0) {
    glUniform4fv ( glGetUniformLocation ( material->program, "LightPosition"),
		   1, 
		   glm::value_ptr(RenderingEnvironment::getInstance().lightPosition) );
  } else {
    // light position XXX
    glm::vec3 tmp;
    tmp = RenderingEnvironment::getInstance().getPointLight(0).getPosition();
    glm::vec4 pos = glm::vec4(tmp, 1.0);
    glUniform4fv ( glGetUniformLocation ( material->program, "LightPosition"),
		   1, 
		   glm::value_ptr(tmp) );
    // light matrix 
    glm::mat4 mtmp = RenderingEnvironment::getInstance().getPointLight(0).getLightMatrix();
    glUniformMatrix4fv ( glGetUniformLocation ( material->program, "LightMatrix"),
			 1, GL_FALSE, 
			 glm::value_ptr(mtmp) );
    // the shadow texture
    // XXX only one light supported
    RenderingEnvironment::getInstance().getPointLight(0).bindShadow(2);
    glUniform1i ( glGetUniformLocation ( material->program, "ShadowMapTexture" ), 
		    2 );
  }

  // time
  glUniform1f ( glGetUniformLocation ( material->program, "Time"),
		glutGet(GLUT_ELAPSED_TIME) / 1000.0 );

  // shadow enable flag
  glUniform1i ( glGetUniformLocation ( material->program, "ShadowEnable"), 
		material->shadowEnable );


  // if ( material->shadowEnable )
  //   glEnable ( GL_POLYGON_OFFSET_FILL );
  // else
  //   glDisable ( GL_POLYGON_OFFSET_FILL );
  
  // fog
  if (RenderingEnvironment::getInstance().getFogEnabled()) {
    glUniform1i ( glGetUniformLocation ( material->program, "FogEnable" ), true );
    glUniform1f ( glGetUniformLocation ( material->program, "FogDensity"),
		  RenderingEnvironment::getInstance().getFogDensity() );
    glUniform4fv ( glGetUniformLocation ( material->program, "FogColor"), 1,
		   glm::value_ptr(RenderingEnvironment::getInstance().getFogColor()) );
    glUniform1f ( glGetUniformLocation ( material->program, "FogStart"),
		  RenderingEnvironment::getInstance().getFogStart() );
    //    glUniform1f ( glGetUniformLocation ( material->program, "FogEnd"),
    //		  RenderingEnvironment::getInstance().getFogEnd() );
    glUniform1i ( glGetUniformLocation ( material->program, "EnableSlabFog" ), 
		  RenderingEnvironment::getInstance().getSlabFogEnabled() );
    // send in the fog planes in eye coords
    glm::vec4 B = transformPlane ( RenderingEnvironment::getInstance().getFogBottomPlane(), modelview );
    glm::vec4 T = transformPlane ( RenderingEnvironment::getInstance().getFogTopPlane(), modelview );
    glUniform4fv ( glGetUniformLocation ( material->program, "FogBottomPlane"),1,
		   glm::value_ptr(B) );
    glUniform4fv ( glGetUniformLocation ( material->program, "FogTopPlane"), 1,
		   glm::value_ptr(T) );
  } else {
    glUniform1i ( glGetUniformLocation ( material->program, "FogEnable" ), false );
  }
}




void
ssg::Primitive::endShader()
{
  // unbind
  glBindBuffer ( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray ( 0 );
  glUseProgram(0); 

}

void
ssg::Primitive::update ( float dt )
{
  if ( parent_ ) {
    Instance *parent = dynamic_cast<Instance*>(parent_);
    if ( parent )
      localToWorld_ = parent->getLocalToWorldMatrix();
    else
      localToWorld_ = glm::mat4();
  }
  else
    localToWorld_ = glm::mat4();
}

void 
ssg::Primitive::draw ( glm::mat4 modelview, 
		       glm::mat4 projection,
		       Ptr<Material> material) {

  if ( isVisible_ ) {

    setupShader ( modelview, projection, material );

    //
    // draw
    //
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, elementBuffer_ );
    glDrawElements( drawingPrimitive_, indices_.size(), GL_UNSIGNED_INT, 0  );
    glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    endShader();
  }
}

glm::mat4 
ssg::Primitive::getWorldToLocalMatrix() {

#if 0
  if ( parent_ )
    return dynamic_cast<Instance*>(parent_)->getWorldToLocalMatrix();
  else
    return glm::mat4();
#endif

  return localToWorld_;
}

glm::mat4
ssg::Primitive::getLocalToWorldMatrix() {
  if ( parent_ ) {
    Instance *parent = dynamic_cast<Instance*>(parent_);
    if ( parent )
      return parent->getLocalToWorldMatrix();
    else
      return glm::mat4();
  }
  else
    return glm::mat4();
}


std::ostream &operator<< (std::ostream &out, const ssg::Primitive &prim)
{
  out << "Primitive\n";
}

//////////////////////////////////////////////////////////////////
// I n s t a n c e
//////////////////////////////////////////////////////////////////

ssg::Instance::~Instance() 
{
  // int n = children_.size();
  // for (int i=0; i < n; i++) {
  //   delete children_[i];
  // }
  //  std::cout << "~Instance\n";
  children_.clear();
}

void
ssg::Instance::print ( int indent ) {
  for (int i=0;i<indent;i++)
    std::cout << "| ";
  std::cout << name_ << " Instance " << std::endl;
  for (int i = 0; i < numChildren(); i++) 
    children_[i]->print ( indent + 1 );
}


void 
ssg::Instance::update ( float dt )  {

  if ( parent_ ) {
    localToWorld_ = dynamic_cast<Instance*>(parent_)->getLocalToWorldMatrix() * matrix_;
    worldToLocal_ = glm::inverse(matrix_) * dynamic_cast<Instance*>(parent_)->worldToLocal_;
  } else {
    localToWorld_ = matrix_;
    worldToLocal_ = glm::inverse(matrix_);
  }

  for (int i=0; i < children_.size(); i++ ) {
    children_[i]->update ( dt );
  }

}

void 
ssg::Instance::draw ( glm::mat4 modelview, 
		      glm::mat4 projection,
		      Ptr<Material> material )   {
  if ( isVisible_ ) {

    // draw every child node
    //    std::vector<ModelNode*>::iterator node;
    std::vector<Ptr<ModelNode> >::iterator node;
    // append the instance transform to the ctm
    glm::mat4 ctm = modelview * matrix_;
    if ( material_.get() && !material.get() )
      material = material_;

    for (int i=0; i < children_.size(); i++ ) {
      children_[i]->draw ( ctm, projection, material );
    }

  }
}

void 
ssg::Instance::setMatrix ( glm::mat4 mat ) {
  matrix_ = mat;
}

glm::mat4 
ssg::Instance::getMatrix () {
  return matrix_;
}


void 
ssg::Instance::addChild ( ModelNode* child ) {
  children_.push_back ( Ptr<ModelNode> (child) );
  child->parent_ = this;
}
void 
ssg::Instance::addChild ( Ptr<ModelNode> child ) {
  children_.push_back ( child );
  child->parent_ = this;
}


// void 
// ssg::Instance::addChild ( Ptr<Instance> child ) {
//   children_.push_back ( child );
//   child->parent_ = this;
// }

// void 
// ssg::Instance::addChild ( Ptr<Primitive> child ) {
//   children_.push_back ( child );
//   child->parent_ = this;
// }


ssg::Ptr<ssg::ModelNode> 
ssg::Instance::getChild ( int i ) {
  if ( i >= 0 && i < children_.size() )
    return children_[i];
  else 
    return 0;
}

int
ssg::Instance::numChildren ()
{
  return children_.size();
}


void 
ssg::Instance::setChild ( ModelNode* child, int slot ) {
  if (slot>0 && slot<children_.size()) {
    children_[slot] = Ptr<ModelNode> (child);
    child->parent_ = this;
  }
}

void 
ssg::Instance::setChild ( Ptr<ModelNode> child, int slot ) {
  if (slot>0 && slot<children_.size()) {
    children_[slot] = child;
    child->parent_ = this;
  }
}

// void 
// ssg::Instance::setChild ( Ptr<Instance> child, int slot ) {
//   if (slot>0 && slot<children_.size()) {
//     p->children_[slot] = child;
//     child->parent_ = this;
//   }
// }

// void 
// ssg::Instance::setChild ( Ptr<Primitive> child, int slot ) {
//   if (slot>0 && slot<children_.size()) {
//     p->children_[slot] = child;
//     child->parent_ = this;
//   }
// }




glm::mat4 
ssg::Instance::getWorldToLocalMatrix() {
  // glm::mat4 m;
  // if (parent_)
  //   m = parent_->getWorldToLocalMatrix();
  // return glm::inverse(matrix_) * m;
  return worldToLocal_;
}

glm::mat4
ssg::Instance::getLocalToWorldMatrix() {
  return localToWorld_;
}

std::ostream &operator<< (std::ostream &out, const ssg::Instance &inst)
{
  out << "Instance\n";
}


///////////////////////////////////////////////////////

//
// a Triangle primitive class
// 

ssg::Triangle::Triangle () {
  points_.push_back ( glm::vec3 ( 0,-1,0 ) );
  points_.push_back ( glm::vec3 ( 1,1,0 ) );
  points_.push_back ( glm::vec3 ( -1,1,0 ) );

  normals_.push_back ( glm::vec3 ( 0,0,1 ) );
  normals_.push_back ( glm::vec3 ( 0,0,1 ) );
  normals_.push_back ( glm::vec3 ( 0,0,1 ) );

  indices_.push_back ( 0 );
  indices_.push_back ( 1 );
  indices_.push_back ( 2 );

  texCoords_.push_back ( glm::vec2 (0.5, 0.0) );
  texCoords_.push_back ( glm::vec2 (1.0, 1.0) );
  texCoords_.push_back ( glm::vec2 (0.0, 1.0) );

  drawingPrimitive_ = GL_TRIANGLES;

  ssg::Primitive::init();
}

//
// a Marker primitive class
// 

ssg::Marker::Marker () {
  float size = 0.05;
  points_.push_back ( glm::vec3 ( -size,0,0 ) );
  points_.push_back ( glm::vec3 (  size,0,0 ) );
  points_.push_back ( glm::vec3 (  0,-size,0 ) );
  points_.push_back ( glm::vec3 (  0, size,0 ) );
  points_.push_back ( glm::vec3 ( 0,0,-size ) );
  points_.push_back ( glm::vec3 ( 0,0, size ) );


  normals_.push_back ( glm::vec3 ( 1,0,0 ) );
  normals_.push_back ( glm::vec3 ( 1,0,0 ) );
  normals_.push_back ( glm::vec3 ( 0,1,0 ) );
  normals_.push_back ( glm::vec3 ( 0,1,0 ) );
  normals_.push_back ( glm::vec3 ( 0,0,1 ) );
  normals_.push_back ( glm::vec3 ( 0,0,1 ) );

  indices_.push_back ( 0 );
  indices_.push_back ( 1 );
  indices_.push_back ( 2 );
  indices_.push_back ( 3 );
  indices_.push_back ( 4 );
  indices_.push_back ( 5 );

  texCoords_.push_back ( glm::vec2 (0.0, 0.0) );
  texCoords_.push_back ( glm::vec2 (1.0, 1.0) );
  texCoords_.push_back ( glm::vec2 (0.0, 0.0) );
  texCoords_.push_back ( glm::vec2 (1.0, 1.0) );
  texCoords_.push_back ( glm::vec2 (0.0, 0.0) );
  texCoords_.push_back ( glm::vec2 (1.0, 1.0) );

  drawingPrimitive_ = GL_LINES;

  ssg::Primitive::init();
}


//
// a Quad primitive
//

ssg::Quad::Quad() {
  glm::vec3 v[4];
  glm::vec2 uv[4];
  v[0] = glm::vec3 (  -1,  -1, 0 );
  v[1] = glm::vec3 (  1,  -1, 0 );
  v[2] = glm::vec3 (  1,  1, 0 );
  v[3] = glm::vec3 (  -1,  1, 0 );

  uv[0] = glm::vec2 (0,0 );
  uv[1] = glm::vec2 (1,0 );
  uv[2] = glm::vec2 (1,1 );
  uv[3] = glm::vec2 (0,1 );

  for (int i = 0; i < 4; i++) {
    points_.push_back(v[i]);
    normals_.push_back ( glm::vec3 (0,0,-1) );
    texCoords_.push_back( uv[i] );
  }

  indices_.push_back ( 0 );
  indices_.push_back ( 1 );
  indices_.push_back ( 2 );
  indices_.push_back ( 3 );

  drawingPrimitive_ = GL_TRIANGLE_FAN;

  ssg::Primitive::init();
}



///////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////

unsigned int
ssg::Material::loadShaders ( const char *basename ) {
	std::string sep("/");
	std::string base(basename);
	std::string vertfile (shaderDir);
	vertfile += sep;
	vertfile += base;
	std::string fragfile ( shaderDir );
	fragfile += sep;
	fragfile += base;
  std::string vfile ( vertfile );
  std::string ffile ( fragfile );
  vfile += ".vert";
  ffile += ".frag";
  return InitShader( vfile.c_str(), ffile.c_str() );
}







 


float 
ssg::urand() {
  return rand() / static_cast<float>(RAND_MAX);
}


std::string 
ssg::ssgPrependDataPath ( const char* basename )
{

  // first test if the pathname exists, if so return it

  std::ifstream testbasename ( basename );
  if ( testbasename )
    return std::string(basename);

  // else prepend the SSG_DATA environment variable

  std::string path;
  std::string dataPath ( getenv("SSG_DATA") );
  if ( dataPath.length() > 0 ) {
    path = dataPath; 
	path += "//";
	path += basename;
  } else {
    //    path = basename;
    std::cout << "ERROR: SSG_DATA environment variable not set!\n";
    exit(1);
  }
  return path;
}

//========================================================================


void 
ssg::RenderingEnvironment::addPointLight ( glm::vec3 pos, glm::vec3 at )
{
  // only one for now, and for backwards compat, wh put this in the old place too
  pointLights.push_back(new PointLight(pos,at));
  lightPosition = glm::vec4(pos,1.0);
  lightColor = glm::vec4(1.0,1.0,1.0,1.0);
}    


//========================================================================

std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec) {
    out << "{" 
        << vec.x << " " << vec.y << " "<< vec.z 
        << "}";

    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) {
    out << "{" 
        << vec.x << " " << vec.y << " "<< vec.z << " " << vec.w
        << "}";

    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat) {
    out << "{" 
        << glm::row<glm::mat4> ( mat, 0 )
        << glm::row<glm::mat4> ( mat, 1 )
        << glm::row<glm::mat4> ( mat, 2 )
        << glm::row<glm::mat4> ( mat, 3 )
        << "}";

    return out;
}


