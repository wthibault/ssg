// 
// ssg.cpp
//  simple scene graph
// (c) William C. Thibault, 2012, 2013
// All Rights Reversed.
//
//
#include "ssg.h"
#include <fstream>

//#ifdef __APPLE__
std::string shaderDir ("shaders120"); // #version 120 (mac)
//#else
//std::string shaderDir ("shaders"); // #version 150
//#endif

#ifdef USE_APPLE_VERTEXARRAYS
#define   glGenVertexArrays glGenVertexArraysAPPLE
#define   glBindVertexArray glBindVertexArrayAPPLE
#endif

//////////////////////////////////////////////////////////////////
// Primitive
//////////////////////////////////////////////////////////////////

void
Primitive::generateAndLoadArrayBuffer()
{
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
  glBufferSubData( GL_ARRAY_BUFFER, sizeofPoints + sizeofNormals, sizeofTexCoords, &texCoords_[0] );
}


void 
Primitive::init () {

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



void
Primitive::setupShader ( glm::mat4 modelview,
			 glm::mat4 projection,
			 Material *material )
{

  if ( !material ) {
    // no material passed in
    if ( !material_ ) {
      // no material on primitive, make one now and keep it
      material_ = new Material;
      material_->ambient = glm::vec4 ( 0.2, 0.2, 0.2, 1.0 );
      material_->diffuse = glm::vec4 ( 0.7, 0.7, 0.7, 1.0 );
      material_->ambient = glm::vec4 ( 0.1, 0.1, 0.1, 1.0 );
      material_->shininess = 200.0;
      material_->program = material_->loadShaders ( "BumpMappedTexturedPhongShading" );
      // XXX default textures
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
  if ( material->diffuseTexture ) {
    material->diffuseTexture->bind(0);
    glUniform1i ( glGetUniformLocation ( material->program, "Texture" ),
		  0 );
  }

  if ( material->bumpTexture ) {
    material->bumpTexture->bind(1);
    glUniform1i ( glGetUniformLocation ( material->program, "BumpTexture" ), 
		  1 );
  }

  

  // load the light info from the rendering environment (same for all prims)
  // XXX take this to world coords 
  glUniform4fv ( glGetUniformLocation ( material->program, "LightPosition"),
		 1, 
		 glm::value_ptr(RenderingEnvironment::getInstance().lightPosition) );
  

}


void
Primitive::endShader()
{
  // unbind
  glBindBuffer ( GL_ARRAY_BUFFER, 0 );
  glBindVertexArray ( 0 );
  glUseProgram(0); 

}

void 
Primitive::draw ( glm::mat4 modelview, 
		  glm::mat4 projection,
		  Material *material) {

  setupShader ( modelview, projection, material );

  //
  // draw
  //
  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, elementBuffer_ );
  glDrawElements( drawingPrimitive_, indices_.size(), GL_UNSIGNED_INT, 0  );
  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0 );

  endShader();
}

void
Primitive::setMaterial ( Material *m ) { 
  material_ = m; 
};

glm::mat4 
Primitive::getWorldToLocalMatrix() {
  return dynamic_cast<Instance*>(parent_)->getWorldToLocalMatrix();
}

//////////////////////////////////////////////////////////////////
// I n s t a n c e
//////////////////////////////////////////////////////////////////

void 
Instance::update ( float dt )  {
  std::vector<ModelNode*>::iterator node;
  if ( parent_ ) {
    localToWorld_ = dynamic_cast<Instance*>(parent_)->localToWorld_ * matrix_;
    worldToLocal_ = glm::inverse(matrix_) * dynamic_cast<Instance*>(parent_)->worldToLocal_;
  }
  for ( node = children_.begin(); node != children_.end(); node++ ) {
    (*node)->update ( dt );
  }
}

void 
Instance::draw ( glm::mat4 modelview, 
		 glm::mat4 projection,
		 Material *material )   {
  // draw every child node
  std::vector<ModelNode*>::iterator node;
  // append the instance transform to the ctm
  glm::mat4 ctm = modelview * matrix_;
  if ( material_ )
    material = material_;
  for ( node = children_.begin(); node != children_.end(); node++ ) {
    (*node)->draw ( ctm, projection, material );
  }
}

void 
Instance::setMatrix ( glm::mat4 mat ) {
  matrix_ = mat;
}

glm::mat4 
Instance::getMatrix () {
  return matrix_;
}

void
Instance::setMaterial ( Material *m ) { 
  material_ = m; 
};

void 
Instance::addChild ( ModelNode * child ) {
  children_.push_back ( child );
  child->parent_ = this;
}

ModelNode* 
Instance::getChild ( int i ) {
  if ( i >= 0 && i < children_.size() )
    return children_[i];
  else 
    return 0;
}

glm::mat4 
Instance::getWorldToLocalMatrix() {
  // glm::mat4 m;
  // if (parent_)
  //   m = parent_->getWorldToLocalMatrix();
  // return glm::inverse(matrix_) * m;
  return worldToLocal_;
}

//
// a Triangle primitive class
// 

Triangle::Triangle () {
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
  texCoords_.push_back ( glm::vec2 (0.0, 1.0) );
  texCoords_.push_back ( glm::vec2 (1.0, 1.0) );

  drawingPrimitive_ = GL_TRIANGLES;

  Primitive::init();
}

//
// a Marker primitive class
// 

Marker::Marker () {
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

  Primitive::init();
}





///////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////

unsigned int
Material::loadShaders ( const char *basename ) {
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
urand() {
  return rand() / static_cast<float>(RAND_MAX);
}


std::string 
ssgPrependDataPath ( const char* basename )
{

  // first test if the pathname exists, if so return it

  std::ifstream testbasename ( basename );
  if ( testbasename )
    return std::string(basename);

  // else prepend the SSG_DATA environment variable

  std::string path;
  std::string dataPath = getenv("SSG_DATA");
  if ( dataPath.length() > 0 ) {
    path = dataPath; 
	path += "//";
	path += basename;
  } else
    path = basename;
  return path;
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


