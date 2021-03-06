/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#ifndef MIR_MIR_TEST_GL_MOCK_H_
#define MIR_MIR_TEST_GL_MOCK_H_

#include <gmock/gmock.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace mir
{
class GLMock
{
public:
    GLMock();
    ~GLMock();
    void silence_uninteresting();

    MOCK_METHOD2(glEGLImageTargetTexture2DOES, void(GLenum, GLeglImageOES));
    MOCK_METHOD1(glGetString, const GLubyte*(GLenum));
    MOCK_METHOD1(glUseProgram, void (GLuint));
    MOCK_METHOD1(glEnable, void (GLenum));
    MOCK_METHOD2(glBlendFunc, void (GLenum, GLenum));
    MOCK_METHOD1(glActiveTexture, void (GLenum));
    MOCK_METHOD4(glUniformMatrix4fv, void (GLuint, GLsizei, GLboolean, const GLfloat *));
    MOCK_METHOD2(glUniform1f, void (GLuint, GLfloat));
    MOCK_METHOD2(glBindBuffer, void (GLenum, GLuint));
    MOCK_METHOD6(glVertexAttribPointer, void (GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *));
    MOCK_METHOD2(glBindTexture, void (GLenum, GLuint));
    MOCK_METHOD1(glEnableVertexAttribArray, void (GLuint));
    MOCK_METHOD1(glDisableVertexAttribArray, void (GLuint));
    MOCK_METHOD3(glDrawArrays, void (GLenum, GLint, GLsizei));
    MOCK_METHOD1(glCreateShader, GLuint (GLenum));
    MOCK_METHOD0(glCreateProgram, GLuint ());
    MOCK_METHOD2(glAttachShader, void (GLuint, GLuint));
    MOCK_METHOD1(glLinkProgram, void (GLuint));
    MOCK_METHOD2(glGetUniformLocation, GLint (GLuint, const GLchar *));
    MOCK_METHOD2(glGetAttribLocation, GLint (GLuint, const GLchar *));
    MOCK_METHOD3(glTexParameteri, void (GLenum, GLenum, GLenum));
    MOCK_METHOD2(glGenTextures, void (GLsizei, GLuint *));
    MOCK_METHOD2(glGenBuffers, void (GLsizei, GLuint *));
    MOCK_METHOD4(glBufferData, void (GLenum, GLsizeiptr, const GLvoid *, GLenum));
    MOCK_METHOD4(glShaderSource, void (GLuint, GLsizei, const GLchar * const *, const GLint *));
    MOCK_METHOD1(glCompileShader, void (GLuint));
    MOCK_METHOD3(glGetShaderiv, void (GLuint, GLenum, GLint *));
    MOCK_METHOD4(glGetShaderInfoLog, void (GLuint, GLsizei, GLsizei *, GLchar *));
    MOCK_METHOD2(glUniform1i, void (GLint, GLint));
    MOCK_METHOD3(glGetProgramiv, void (GLuint, GLenum, GLint *));
    MOCK_METHOD4(glGetProgramInfoLog, void (GLuint, GLsizei, GLsizei *, GLchar *));
};
}

#endif /* MIR_MIR_TEST_GL_MOCK_H_ */
