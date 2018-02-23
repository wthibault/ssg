//
// Texture.cpp
//  modified from https://github.com/mortennobel/OpenGL_3_2_Utils
//  William.Thibault@csueastbay.edu
//
/*!
 * OpenGL 3.2 Utils - Extension to the Angel library (from the book Interactive Computer Graphics 6th ed
 * https://github.com/mortennobel/OpenGL_3_2_Utils
 *
 * New BSD License
 *
 * Copyright (c) 2011, Morten Nobel-Joergensen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
 * disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "Texture.h"
#include "TextureLoader.h"
#include "InitShader.h"

#include <iostream>

using namespace std;
using namespace glm;

ssg::Texture::Texture(GLuint width, GLuint height, bool floatingPoint, bool mipmaps, unsigned int t )
  : width(width), 
    height(height), 
    floatingPoint(floatingPoint), 
    drawTextureShader(0),
    drawTextureVertexArrayObject(0),
    drawTextureUniform(0),
    count_(0)
{
  glActiveTexture(GL_TEXTURE0 + t);
  setupTexParams( floatingPoint, mipmaps );
    
  if (drawTextureShader == 0){
    setupRenderFullscreenQuad();
  }

  unbind(t);
}


ssg::Texture::Texture(const char *bmpfilename, bool floatingPoint, bool mipmaps, unsigned int texUnit, bool repeat )
  : floatingPoint(floatingPoint), count_(0)
{

  if ( floatingPoint ) {
    std::cout << "texture loading not supported for floating point\n" ;
    return;
  }

  // load from file ( convert to RGBA )
  //  unsigned char *rgbData = loadBMPRaw ( bmpfilename, width, height, false );
  unsigned char *rgbData = loadImage ( bmpfilename, width, height );
  if ( !rgbData ) {
    std::cout << "texture load failed for " << bmpfilename << ", using dummy texture" << std::endl;
    rgbData = (unsigned char *) malloc ( 3 );
    rgbData[0] = rgbData[1] = rgbData[2] = 128;
    width = 1;
    height = 1;
  }
  unsigned char *rgbaData = new unsigned char [width*height*4];
  unsigned char *p = rgbaData;
  unsigned char *q = rgbData;
  for ( int i = 0; i < width*height; i++ ) {
    *p++ = *q++;
    *p++ = *q++;
    *p++ = *q++;
    *p++ = 255;
  }

  glActiveTexture ( GL_TEXTURE0 + texUnit );
  // setup the texture
  setupTexParams ( floatingPoint, mipmaps, repeat );

  // upload texture data
  glBindTexture ( GL_TEXTURE_2D, textureId );
  glTexSubImage2D ( GL_TEXTURE_2D, 
		    0, 
		    0,0,
		    width, height,
		    //		    GL_BGRA,
		    GL_RGBA,
		    GL_UNSIGNED_BYTE,
		    static_cast<GLvoid*>(rgbaData) );

  if ( mipmaps ) 
    generateMipmaps();

  glBindTexture ( GL_TEXTURE_2D, 0 );

  if (drawTextureShader == 0){
    setupRenderFullscreenQuad();
  }

  unbind(texUnit);
  delete rgbaData;
  delete rgbData;
}


void
ssg::Texture::setupTexParams( bool floatingpoint, bool mipmaps, bool repeat )
{
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
		  GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		  mipmaps?GL_LINEAR_MIPMAP_LINEAR:GL_LINEAR);

  if (repeat) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // use clamping for fx..., repeat for diffusemap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  }

  // allocate the memory for the texels
  //  unsigned char *data = new unsigned char[width * height * 4 * floatingpoint?sizeof(GLfloat):1];
  glTexImage2D(GL_TEXTURE_2D, 0, 
	       floatingPoint?GL_RGBA32F_ARB:GL_RGBA8, 
	       width, height, 
	       0,
	       GL_RGBA, 
	       floatingPoint?GL_FLOAT:GL_UNSIGNED_BYTE, 
	       0);
	       //	       data);
  //  delete [] data;
  loadZeros(0);
}

ssg::Texture::~Texture(){
  glDeleteTextures(1, &textureId);
}

void 
ssg::Texture::renderFullscreenQuad(){
  glUseProgram(drawTextureShader);
  glBindVertexArray(drawTextureVertexArrayObject);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glUniform1i(drawTextureUniform, 0);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}

void 
ssg::Texture::setupRenderFullscreenQuad(const char* vprog, const char* fprog){
  //  drawTextureShader = InitShader("shaders120/fullscreentexture.vert",  
  //				 "shaders120/fullscreentexture.frag");

  drawTextureShader = InitShader( vprog, fprog );


  GLuint positionAttributeLocation = glGetAttribLocation(drawTextureShader, 
							 "position");
  drawTextureUniform = glGetUniformLocation(drawTextureShader, 
					    "texture1");
    
  glGenVertexArrays(1, &drawTextureVertexArrayObject);
  glBindVertexArray(drawTextureVertexArrayObject);
    
  vec2 array[3] = { // triangle fill screen in clip space
    vec2(-1,-1),
    vec2(3,-1),
    vec2(-1,3)
  };
    
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(vec2), &(array[0]), GL_STATIC_DRAW);
	
  glEnableVertexAttribArray(positionAttributeLocation);
  glVertexAttribPointer(positionAttributeLocation, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (const GLvoid *)(0));

  //  std::cout << "setupRenderFullscreenQuad(" << vprog << "," << fprog << "):" 
  //	    << "shader: " << drawTextureShader
  //	    << std::endl;
}

void 
ssg::Texture::generateMipmaps(){
  glBindTexture(GL_TEXTURE_2D, textureId);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void 
ssg::Texture::bind(unsigned int t)
{
  glActiveTexture ( GL_TEXTURE0 + t );
  glBindTexture ( GL_TEXTURE_2D, textureId );
}

void 
ssg::Texture::unbind(unsigned int t)
{
  glActiveTexture ( GL_TEXTURE0 + t );
  glBindTexture ( GL_TEXTURE_2D, 0 );
}

void 
ssg::Texture::loadChecks(unsigned int t)
{
  unsigned char *data = NULL;

  glActiveTexture(GL_TEXTURE0 + t);
  if ( floatingPoint ) {
    //    std::cout << "checks not supported for floating point\n" ;
    //    return;
    data = new unsigned char [width * height * 4 * sizeof(float)];
    float *p = (float*)data;
    // fill with checks
    for ( int row=0;row<height;row++ ) {
      for ( int col=0; col<width; col++ ) {
	float c = (((((row&0x80)==0)^((col&0x80)==0))==0));
	*p++ = c;
	*p++ = c;
	*p++ = c;
	*p++ = 1.0;
      }
    }
  }
  else {
    data = new unsigned char [width * height * 4];
    unsigned char *p = data;
    // fill with checks
    for ( int row=0;row<height;row++ ) {
      for ( int col=0; col<width; col++ ) {
	unsigned char c = (((((row&0x80)==0)^((col&0x80)))==0))*255;
	*p++ = c;
	*p++ = c;
	*p++ = c;
	*p++ = 255;
      }
    }
  }

  // upload texture data
  glBindTexture ( GL_TEXTURE_2D, textureId );

  glTexSubImage2D ( GL_TEXTURE_2D, 
		    0, 
		    0,0,
		    width, height,
		    GL_RGBA,
		    floatingPoint?GL_FLOAT:GL_UNSIGNED_BYTE,
		    static_cast<GLvoid*>(data) );

  generateMipmaps();

  glBindTexture ( GL_TEXTURE_2D, 0 );
  delete [] data;
}


void 
ssg::Texture::loadZeros(unsigned int t)
{
  unsigned char *data = NULL;

  glActiveTexture(GL_TEXTURE0 + t);
  if ( floatingPoint ) {
    data = new unsigned char [width * height * 4 * sizeof(float)];
    float *p = (float*)data;
    // fill with 0
    for ( int row=0;row<height;row++ ) {
      for ( int col=0; col<width; col++ ) {
	float c = 0;
	*p++ = c;
	*p++ = c;
	*p++ = c;
	*p++ = 1.0;
      }
    }
  }
  else {
    data = new unsigned char [width * height * 4];
    unsigned char *p = data;
    // fill with 0
    for ( int row=0;row<height;row++ ) {
      for ( int col=0; col<width; col++ ) {
	unsigned char c = 0;
	*p++ = c;
	*p++ = c;
	*p++ = c;
	*p++ = 255;
      }
    }
  }

  // upload texture data
  glBindTexture ( GL_TEXTURE_2D, textureId );

  glTexSubImage2D ( GL_TEXTURE_2D, 
		    0, 
		    0,0,
		    width, height,
		    GL_RGBA,
		    floatingPoint?GL_FLOAT:GL_UNSIGNED_BYTE,
		    static_cast<GLvoid*>(data) );

  generateMipmaps();

  glBindTexture ( GL_TEXTURE_2D, 0 );
  delete [] data;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//
// SHADOW TEXTURE
//


#ifdef HIGHRES_SHADOWS
const int ssg::ShadowTexture::SHADOW_WIDTH = 8192;
//const int ShadowTexture::SHADOW_WIDTH = 4096;
#else
const int ssg::ShadowTexture::SHADOW_WIDTH = 2048;
#endif

ssg::ShadowTexture::ShadowTexture ()
{
  glGenTextures(1,&textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );

  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  //  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  GLfloat ones[]={1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, ones);

#if 0
  // create a test shadow (debug)

  GLfloat *testTex = new GLfloat [SHADOW_WIDTH*SHADOW_WIDTH];
  for (int i = 0; i < SHADOW_WIDTH; i++ ) {
    for (int j = 0; j < SHADOW_WIDTH; j++ ) {
      //      testTex[i*SHADOW_WIDTH+j] = i * 1.0f / SHADOW_WIDTH;
      testTex[i*SHADOW_WIDTH+j] = (GLfloat)rand();
    }
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	       SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)testTex );

#else

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
	       SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );

#endif

  glBindTexture(GL_TEXTURE_2D, 0);
}


