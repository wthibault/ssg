// FrameBufferObject.h
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

#ifndef __VertexBlending__FrameBufferObject__
#define __VertexBlending__FrameBufferObject__

#include <iostream>
//#include "Angel.h"

#include "Texture.h"

// Simplifies render to texture
// Example usage:
//
// // ---- Setup
// Texture *offscreenTexture = new Texture(WINDOW_WIDTH, WINDOW_HEIGHT); 
// FrameBufferObject *fbo = new FrameBufferObject(offscreenTexture);
//
// // ----- Usage
// fbo->bind();
// glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
// // do your rendering
// fbo->unbind();
// // offscreenTexture now contains the rendering
// 
class FrameBufferObject {
public:
    FrameBufferObject(Texture *texture, bool useDepthBuffer = true);
    FrameBufferObject(Texture *texture, ShadowTexture *depthTexture);
    ~FrameBufferObject();
    // bind the framebufferobject as current
    void bind();
    // releases the framebuffer object (this is not needed if another framebuffer object is bound instead)
    void unbind();
    void getSize ( GLuint &width, GLuint &height ) {
      width = texture->getWidth();
      height = texture->getHeight();
    }
private:
    void checkFramebufferStatusOk();
    Texture *texture;
    GLuint framebufferid;
};



#endif /* defined(__VertexBlending__FrameBufferObject__) */
