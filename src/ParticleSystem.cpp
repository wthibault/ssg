// 
// ParticleSystem.cpp
//

#include "ParticleSystem.h"
using namespace ssg;

glm::vec3 ParticleSystem::gravityCenter (0,0,0);

void
ssg::ParticleSystem::init (  ) 
{
  // initialize and clear
  pos.clear();
  oldpos.clear();
  vel.clear();
  age.clear();

  generators.push_back(glm::vec3(0,0,0));

  // load with dead particles
  for ( int i = 0; i < MAX_PARTICLES; i++ ) {
    pos.push_back ( glm::vec3(0,0,0) );
    oldpos.push_back ( glm::vec3(0,0,0) );
    vel.push_back ( glm::vec3(0,0,1) );
    age.push_back ( MAX_AGE + 1 );
    points_.push_back ( glm::vec3(0,0,0) );
    points_.push_back ( glm::vec3(0,0,0) ); // twice in case lines
    normals_.push_back ( glm::vec3(0,0,1) );
    indices_.push_back ( i );
    texCoords_.push_back ( glm::vec2 ( 0,0 ) );
  }

  drawingPrimitive_ = GL_POINTS;

  Primitive::init();
}

int 
ParticleSystem::findFreeSlot ( int slot )
{
  while ( slot < age.size() && age[slot] <= MAX_AGE ) 
    slot++;
  if ( slot == age.size() ) 
    return -1;
  return slot;
}


glm::vec3
ParticleSystem::initialVelocity() 
{
  glm::vec3 noise ( urand()-0.5,urand()-0.5,urand()-0.5 );
  noise *= NOISE_LEVEL * INITIAL_SPEED;
  glm::vec3 v ( 1,0,0 );
  v *= INITIAL_SPEED;
  return v + noise;
}


glm::vec3
ParticleSystem::accelerationAt ( glm::vec3 p )
{
  const float EPSILON = 1.0e-6;
  if ( parent_ ) {
    glm::mat4 m = parent_->getWorldToLocalMatrix();
    glm::vec4 v ( gravityCenter, 1.0 );
    glm::vec4 vv = m * v;
    glm::vec3 gravCenter ( vv.x, vv.y, vv.z );
    glm::vec3 d = gravCenter - p;
    float r = glm::length(d);
    if ( fabs(r) < EPSILON )
      r = EPSILON;
    float G = 1.0;
    glm::vec3 accel = G * d;
    accel /= r*r*r*r;
    //    std::cout << accel << std::endl;
    return accel;
  } else {
    return glm::vec3 ( 0,0,0) - p;
  }
}


void
ParticleSystem::update ( float dt ) 
{
  // create new particles
  int slot=0;
  for ( int i = 0; i < generators.size(); i++ ) {
      for ( int k = 0; k < PARTICLES_PER_FRAME_PER_GENERATOR; k++ ) {
	slot = findFreeSlot ( slot );
	if (slot == -1) {
	  continue;
	}
	pos[slot] = generators[i];
	oldpos[slot] = pos[slot];
	vel[slot] = initialVelocity();
	pos[slot] += vel[slot] * urand() * 0.033f; // offset within a frame.
	age[slot] = 0.0;
      }
    }
  // update particles.
  for ( int i = 0; i < pos.size(); i++ ) { 
    if ( age[i] <= MAX_AGE ) {
      age[i] += dt;
      vel[i] += accelerationAt ( pos[i] ) * static_cast<float>(dt);
      float speed = glm::length(vel[i]);
      if (speed > MAX_SPEED )
	vel[i] = glm::normalize(vel[i]) * MAX_SPEED;
      oldpos[i] = pos[i];
      pos[i] += vel[i] * static_cast<float>(dt);
    }
  }
}
  
void
ParticleSystem::draw ( glm::mat4 mv,
		       glm::mat4 proj,
		       Ptr<Material> mat) 
{

  // copy the positions of living points to the vertex array
  glBindVertexArray ( vao_ );

  points_.clear();
  indices_.clear();
  normals_.clear();
  texCoords_.clear();

#ifdef DRAW_POINTS
  int index = 0;
  for ( int i = 0; i < pos.size(); i++ ) {
    if ( age[i] < MAX_AGE ) { 
      points_.push_back( pos[i] );
      normals_.push_back ( glm::vec3(0,0,1) );
      texCoords_.push_back ( glm::vec2(0,0) );
      //      indices_.push_back ( i );
      indices_.push_back ( index++ );
    }
  }
  long int sizeofPoints = sizeof(glm::vec3)*points_.size();
  int sizeofNormals = sizeof(glm::vec3)*normals_.size();
  int sizeofTexCoords = sizeof(glm::vec2)*texCoords_.size();
  drawingPrimitive_ = GL_POINTS;
#endif

#define DRAW_LINES
#ifdef DRAW_LINES
  int index = 0;
  drawingPrimitive_ = GL_LINES;
  for ( int i = 0; i < pos.size(); i++ ) {
    if ( age[i] < MAX_AGE ) { 
      
      points_.push_back( oldpos[i] );
      points_.push_back( pos[i] );
      normals_.push_back ( glm::vec3(0,0,1) );
      texCoords_.push_back ( glm::vec2(0,0) );
      indices_.push_back ( index++ );
      indices_.push_back ( index++ );
    }
  }
  long int sizeofPoints = sizeof(glm::vec3)*points_.size()*2;
  int sizeofNormals = sizeof(glm::vec3)*normals_.size();
  int sizeofTexCoords = sizeof(glm::vec2)*texCoords_.size();
#endif

  glBindBuffer ( GL_ARRAY_BUFFER, arrayBuffer_ );
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeofPoints, &points_[0] );
  glBufferSubData( GL_ARRAY_BUFFER, sizeofPoints, sizeofNormals, &normals_[0] );
  glBufferSubData( GL_ARRAY_BUFFER, sizeofPoints + sizeofNormals, sizeofTexCoords, &texCoords_[0] );
  glBindBuffer ( GL_ARRAY_BUFFER, 0 );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer_);
  int sizeofIndices = indices_.size()*sizeof(unsigned int);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeofIndices, &indices_[0]);
  glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  Primitive::draw ( mv, proj, mat );
  
}

