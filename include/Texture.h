// Texture.h
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

#ifndef __VertexBlending__Texture__
#define __VertexBlending__Texture__

#include <iostream>

//#include "Angel.h"
//#include "DemoModule.h"
#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif

#ifdef __APPLE__
#define    glGenVertexArrays glGenVertexArraysAPPLE
#define    glBindVertexArray glBindVertexArrayAPPLE
#endif

namespace ssg {

// Create a texture
class Texture {
public:
  Texture(){};
  Texture(GLuint width, GLuint height, bool floatingPoint = false, bool mipmaps = false,
	  unsigned int texUnit = 0);
    Texture(const char* bmpfilename, bool floatingPoint = false,
	    bool mipmaps = false,
	    unsigned int texUnit = 0,
	    bool repeat = false);
    ~Texture();
    // Render the texture the the screen
    void renderFullscreenQuad();
    GLuint getHeight() { return height; }
    GLuint getWidth() { return width; }
    GLuint getTextureId() { return textureId; }
    // Explicit generate mipmaps
    void generateMipmaps();
    // load checkerboard image
    void loadChecks(unsigned int texUnit);
    void loadZeros(unsigned int texUnit);
    //    void loadBMP( const char* filename );
    void bind(unsigned int texUnit);
    void unbind(unsigned int texUnit);
    //private:
    void setupRenderFullscreenQuad(const char* vprog="shaders120/fullscreentexture.vert",
				   const char* fprog="shaders120/fullscreentexture.frag");
    void setupTexParams(bool floatingPoint, bool mipmaps, bool repeat = false);
    GLuint textureId;
    bool floatingPoint;
    GLuint width;
    GLuint height;
    GLuint drawTextureShader;
    GLuint drawTextureVertexArrayObject;
    GLuint drawTextureUniform;
};


class ShadowTexture : public Texture
{
 public:
  static const int SHADOW_WIDTH;
  ShadowTexture ();
  ~ShadowTexture();
  GLuint getTextureId() { return textureId; }
};

}; // namespace ssg

#endif /* defined(__VertexBlending__Texture__) */
