// 
// ParticleSystem.h
//
#pragma once

#include "ssg.h"

class ParticleSystem : public Primitive {
public:
  std::vector<glm::vec3> pos, oldpos;
  std::vector<glm::vec3> vel;
  std::vector<float> age;
  std::vector<glm::vec3> generators;
  double MAX_AGE;
  float  INITIAL_SPEED;
  int    PARTICLES_PER_FRAME_PER_GENERATOR;
  int    MAX_PARTICLES;
  float  NOISE_LEVEL;
  float  MAX_SPEED;

 ParticleSystem() :
  MAX_AGE(5.0),
    INITIAL_SPEED(0.8),
    PARTICLES_PER_FRAME_PER_GENERATOR(6),
    MAX_PARTICLES (10000),
    NOISE_LEVEL ( 0.4 ),
    MAX_SPEED ( 6.0 )
      {
	ParticleSystem::init();
      }

  virtual  void init();
  virtual  void update(float dt);
  virtual  void draw (glm::mat4 mv,
	     glm::mat4 proj,
	     Material *mat);

  int findFreeSlot ( int slot );
  glm::vec3 initialVelocity();
  glm::vec3 accelerationAt ( glm::vec3 pos );

  static glm::vec3 gravityCenter;

};
