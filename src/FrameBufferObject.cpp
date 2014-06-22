// FrameBufferObject.cpp
//  modified from https://github.com/mortennobel/OpenGL_3_2_Utils
//  William.Thibault@csueastbay.edu
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

#include "ssg.h"
#include "FrameBufferObject.h"

#ifndef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#endif


#include <iostream>

using namespace std;

FrameBufferObject::FrameBufferObject(Texture *texture, bool useDepthBuffer)
:texture(texture){
    
    
    // create a renderbuffer object to store depth info
    GLuint rboId;
    if (useDepthBuffer){
        glGenRenderbuffers(1, &rboId);
        glBindRenderbuffer(GL_RENDERBUFFER, rboId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          texture->getWidth(), texture->getHeight());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    
    //    std::cout << "post depth\n";
    //    checkFramebufferStatusOk();

    // create a framebuffer object

    glGenFramebuffers(1, &framebufferid);
    bind();

    //n    std::cout << "post gen\n";
    //    checkFramebufferStatusOk();

    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture->getTextureId(), 0);
    

    //    std::cout << "post tex2d\n";
    //    checkFramebufferStatusOk();

    // attach the renderbuffer to depth attachment point
    if (useDepthBuffer){
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rboId);
    }
    //    std::cout << "post renderbuffer\n";
    checkFramebufferStatusOk();
    unbind();
}


FrameBufferObject::FrameBufferObject(Texture *texture, ShadowTexture *depthTexture)
  :texture(texture)
{
    
  if (!texture || !depthTexture) {
    std::cout << "FrameBufferObject gets no textures!!!";
    return;
  }
    
  // create a framebuffer object

  glGenFramebuffers(1, &framebufferid);
  bind();

  //    std::cout << "post gen\n";
  //    checkFramebufferStatusOk();

  // attach the texture to FBO color attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, texture->getTextureId(), 0);
    

  //    std::cout << "post tex2d\n";
  //    checkFramebufferStatusOk();

  // attach the depth texture to depth attachment point
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			 GL_TEXTURE_2D, depthTexture->getTextureId(), 0);

  //    std::cout << "post renderbuffer\n";
  checkFramebufferStatusOk();
  unbind();
}


FrameBufferObject::~FrameBufferObject(){
    glDeleteFramebuffers(1, &framebufferid);
}

void FrameBufferObject::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferid);
}

void FrameBufferObject::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferObject::checkFramebufferStatusOk(){
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status){
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT​ is returned if any of the framebuffer attachment points are framebuffer incomplete."<<endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT​ is returned if the framebuffer does not have at least one image attached to it."<<endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER​ is returned if the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE​ is GL_NONE​ for any color attachment point(s) named by GL_DRAWBUFFERi​."<<endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER​ is returned if GL_READ_BUFFER​ is not GL_NONE​ and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE​ is GL_NONE​ for the color attachment point named by GL_READ_BUFFER."<<endl;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            cout << "GL_FRAMEBUFFER_UNSUPPORTED​ is returned if the combination of internal formats of the attached images violates an implementation-dependent set of restrictions."<<endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE​ is returned if the value of GL_RENDERBUFFER_SAMPLES​ is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES​ is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES​ does not match the value of GL_TEXTURE_SAMPLES.\n"<<
            "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE​ is also returned if the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS​ is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS​ is not GL_TRUE​ for all attached textures."<<endl;
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS​ is returned if any framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target."<<endl;
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            cout << " GL_FRAMEBUFFER_UNDEFINED​ is returned if target​ is the default framebuffer, but the default framebuffer does not exist."<<endl;
            break;
        case GL_FRAMEBUFFER_COMPLETE:
	  //            cout << "Framebuffer complete" << endl;
            break;
    }
}
