//
// PointLight.h -- includes shadows
//

#pragma once

#include "Texture.h"

namespace ssg
{

class PointLight
{
 protected:
  ssg::Camera        lightCam;
  glm::vec3          color;
  FrameBufferObject *shadowMapFBO;
  ShadowTexture     *shadowMap;
  Texture           *image;

 public:

  void      setPosition(glm::vec3 pos) { lightCam.setPosition(pos); }
  glm::vec3 getPosition() { return lightCam.getPosition(); }


  void      setPointAt(glm::vec3 pointat) { lightCam.setLookat(pointat); }
  glm::vec3 getPointAt() { return lightCam.getLookat(); }


  void      setColor(glm::vec3 newcolor) { color=newcolor; }
  glm::vec3 getColor() { return color; }


  void      bindShadow(unsigned int t)
  {
    glActiveTexture ( GL_TEXTURE0 + t );
    glBindTexture ( GL_TEXTURE_2D, shadowMap->getTextureId() );
  }


  ShadowTexture *getShadowTexture () { return shadowMap; }


  ssg::Camera &getLightCamera() { return lightCam; }


  glm::mat4 getLightMatrix()
    {
      glm::mat4 offsetMatrix ( glm::vec4(0.5, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, 0.5, 0.0, 0.0),
			       glm::vec4(0.0, 0.0, 0.5, 0.0),
			       glm::vec4(0.5, 0.5, 0.5, 1.0) );  // columns cf. Eisemann etal p 42.
      return offsetMatrix * lightCam.getProjectionMatrix() * lightCam.getModelviewMatrix();
    }


  void updateShadow ( ssg::ModelNode *shadowCasters )
  {

    // bind FBO as renderbuffer
    shadowMapFBO->bind(); 

    glPolygonOffset(2.5f, 10.0f);
    glEnable(GL_POLYGON_OFFSET_FILL);

    // Disable writes to the color buffer
    //    glDrawBuffer(GL_NONE);
    //    glReadBuffer(GL_NONE);

    // render from light
    glClearColor(0,0,0,1);
    glEnable ( GL_DEPTH_TEST );

    glClear( GL_DEPTH_BUFFER_BIT);
    lightCam.draw(shadowCasters); // need to use our own shader?????

    glDisable(GL_POLYGON_OFFSET_FILL);

    // unbind FBO
    shadowMapFBO->unbind();

  };
  

 PointLight(glm::vec3 pos, glm::vec3 at) 
   : color(1,1,1) 
    {
      std::cout << "PointLight::PointLight !!!\n";
      // setup light geometry
      setPosition(pos);
      setPointAt(at);
      lightCam.setupPerspective ( ShadowTexture::SHADOW_WIDTH, ShadowTexture::SHADOW_WIDTH );


      // std::cout << "lightCam modelview: " << lightCam.getModelviewMatrix() << std::endl;
      // std::cout << "lightCam projection: " << lightCam.getProjectionMatrix() << std::endl;
      // std::cout << "LightMatrix: " << getLightMatrix() << std::endl;

      // setup shadow map FBO
      // can we render depth without color?
      //      image = new Texture ( ShadowTexture::SHADOW_WIDTH, ShadowTexture::SHADOW_WIDTH, false, false, 2 ); // texture unit 2! (do we need this??XXX)
      image = NULL; // don't need it!
      shadowMap = new ShadowTexture ();
      shadowMapFBO = new FrameBufferObject (image, shadowMap);
    };


  ~PointLight() 
    {
      // delete it all
      //      delete image;
      delete shadowMap;
      delete shadowMapFBO;
    };
  
};

};
