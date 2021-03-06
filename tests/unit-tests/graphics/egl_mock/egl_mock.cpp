/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by:
 * Thomas Voss <thomas.voss@canonical.com>
 * Kevin DuBois <kevin.dubois@canonical.com>
 */

#include "mir_test/egl_mock.h"
#include <gtest/gtest.h>
namespace
{
mir::EglMock* global_egl_mock = NULL;
}


EGLConfig configs[] =
{
    (void*)3,
    (void*)4,
    (void*)8,
    (void*)14
};
EGLint config_size = 4;

/* EGL{Surface,Display,Config,Context} are all opaque types, so we can put whatever
   we want in them for testing */
mir::EglMock::EglMock()
    : fake_egl_display((EGLDisplay) 0x0530),
      fake_configs(configs),
      fake_configs_num(config_size),
      fake_egl_surface((EGLSurface) 0xa034),
      fake_egl_context((EGLContext) 0xbeef),
      fake_egl_image((EGLImageKHR) 0x1234),
      fake_visual_id(5)
{
    using namespace testing;
    assert(global_egl_mock == NULL && "Only one mock object per process is allowed");

    global_egl_mock = this;

    ON_CALL(*this, eglGetDisplay(_))
    .WillByDefault(Return(fake_egl_display));
    ON_CALL(*this, eglInitialize(_,_,_))
    .WillByDefault(DoAll(
                       SetArgPointee<1>(1),
                       SetArgPointee<2>(4),
                       Return(EGL_TRUE)));
    ON_CALL(*this, eglBindApi(EGL_OPENGL_ES_API))
    .WillByDefault(Return(EGL_TRUE));

    ON_CALL(*this, eglGetConfigs(_,NULL, 0, _))
    .WillByDefault(DoAll(
                       SetArgPointee<3>(config_size),
                       Return(EGL_TRUE)));

    ON_CALL(*this, eglGetConfigAttrib(_, _, EGL_NATIVE_VISUAL_ID, _))
    .WillByDefault(DoAll(
                       SetArgPointee<3>(fake_visual_id),
                       Return(EGL_TRUE)));

    ON_CALL(*this, eglChooseConfig(_,_,_,_,_))
    .WillByDefault(DoAll(
                       SetArgPointee<2>(&fake_configs),
                       SetArgPointee<4>(fake_configs_num),
                       Return(EGL_TRUE)));

    ON_CALL(*this, eglCreateWindowSurface(_,_,_,_))
    .WillByDefault(Return(fake_egl_surface));

    ON_CALL(*this, eglCreatePbufferSurface(_,_,_))
    .WillByDefault(Return(fake_egl_surface));

    ON_CALL(*this, eglCreateContext(_,_,_,_))
    .WillByDefault(Return(fake_egl_context));

    ON_CALL(*this, eglMakeCurrent(_,_,_,_))
    .WillByDefault(Return(EGL_TRUE));

    ON_CALL(*this, eglSwapBuffers(_,_))
    .WillByDefault(Return(EGL_TRUE));

    ON_CALL(*this, eglGetCurrentDisplay())
    .WillByDefault(Return(fake_egl_display));

    ON_CALL(*this, eglCreateImageKHR(_,_,_,_,_))
    .WillByDefault(Return(fake_egl_image));
}

mir::EglMock::~EglMock()
{
    global_egl_mock = NULL;
}

void mir::EglMock::silence_uninteresting()
{
    using namespace testing;
    EXPECT_CALL(*this, eglGetCurrentDisplay())
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglGetDisplay(_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglGetDisplay(_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglInitialize(_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglGetConfigs(_,_,_, _))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglGetConfigAttrib(_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglChooseConfig(_,_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglCreateWindowSurface(_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglCreatePbufferSurface(_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglCreateContext(_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglMakeCurrent(_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglDestroyContext(_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglDestroySurface(_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglTerminate(_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglSwapBuffers(_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglCreateImageKHR(_,_,_,_,_))
        .Times(AtLeast(0));
    EXPECT_CALL(*this, eglDestroyImageKHR(_,_))
        .Times(AtLeast(0));
}

#define CHECK_GLOBAL_MOCK(rettype)  \
    if (!global_egl_mock)               \
    {                               \
        using namespace ::testing;  \
        ADD_FAILURE_AT(__FILE__,__LINE__); \
        rettype type = (rettype) 0;       \
        return type;        \
    }

EGLint eglGetError (void)
{
    CHECK_GLOBAL_MOCK(EGLint)
    return global_egl_mock->eglGetError();
}

EGLDisplay eglGetDisplay (NativeDisplayType display)
{
    CHECK_GLOBAL_MOCK(EGLDisplay);
    return global_egl_mock->eglGetDisplay(display);
}

EGLBoolean eglInitialize (EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglInitialize(dpy, major, minor);
}

EGLBoolean eglTerminate (EGLDisplay dpy)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglTerminate(dpy);
}

const char * eglQueryString (EGLDisplay dpy, EGLint name)
{
    CHECK_GLOBAL_MOCK(const char *)
    return global_egl_mock->eglQueryString(dpy, name);
}

EGLBoolean eglBindAPI (EGLenum api)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglBindApi(api);
}

mir::EglMock::generic_function_pointer_t eglGetProcAddress (const char *name)
{
    CHECK_GLOBAL_MOCK(mir::EglMock::generic_function_pointer_t)
    return global_egl_mock->eglGetProcAddress(name);
}

EGLBoolean eglGetConfigs (EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglGetConfigs(dpy, configs, config_size, num_config);
}

EGLBoolean eglChooseConfig (EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglChooseConfig(dpy, attrib_list, configs, config_size, num_config);
}

EGLBoolean eglGetConfigAttrib (EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglGetConfigAttrib(dpy, config, attribute, value);
}

EGLSurface eglCreateWindowSurface (EGLDisplay dpy, EGLConfig config, NativeWindowType window, const EGLint *attrib_list)
{
    CHECK_GLOBAL_MOCK(EGLSurface)
    return global_egl_mock->eglCreateWindowSurface(dpy, config, window, attrib_list);
}

EGLSurface eglCreatePixmapSurface (EGLDisplay dpy, EGLConfig config, NativePixmapType pixmap, const EGLint *attrib_list)
{
    CHECK_GLOBAL_MOCK(EGLSurface)
    return global_egl_mock->eglCreatePixmapSurface(dpy, config, pixmap, attrib_list);
}

EGLSurface eglCreatePbufferSurface (EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
    CHECK_GLOBAL_MOCK(EGLSurface)
    return global_egl_mock->eglCreatePbufferSurface(dpy, config, attrib_list);
}

EGLBoolean eglDestroySurface (EGLDisplay dpy, EGLSurface surface)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglDestroySurface(dpy, surface);
}

EGLBoolean eglQuerySurface (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglQuerySurface(dpy, surface, attribute, value);
}

/* EGL 1.1 render-to-texture APIs */
EGLBoolean eglSurfaceAttrib (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglSurfaceAttrib(dpy, surface, attribute, value);
}

EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglBindTexImage(dpy, surface, buffer);
}

EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglReleaseTexImage(dpy, surface, buffer);
}

/* EGL 1.1 swap control API */
EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglSwapInterval(dpy, interval);
}

EGLContext eglCreateContext (EGLDisplay dpy, EGLConfig config, EGLContext share_list, const EGLint *attrib_list)
{
    CHECK_GLOBAL_MOCK(EGLContext)
    return global_egl_mock->eglCreateContext(dpy, config, share_list, attrib_list);
}

EGLBoolean eglDestroyContext (EGLDisplay dpy, EGLContext ctx)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglDestroyContext(dpy, ctx);
}

EGLBoolean eglMakeCurrent (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglMakeCurrent(dpy, draw, read, ctx);
}

EGLContext eglGetCurrentContext (void)
{
    CHECK_GLOBAL_MOCK(EGLContext)
    return global_egl_mock->eglGetCurrentContext();
}

EGLSurface eglGetCurrentSurface (EGLint readdraw)
{
    CHECK_GLOBAL_MOCK(EGLSurface)
    return global_egl_mock->eglGetCurrentSurface(readdraw);
}

EGLDisplay eglGetCurrentDisplay (void)
{
    CHECK_GLOBAL_MOCK(EGLDisplay)
    return global_egl_mock->eglGetCurrentDisplay();
}

EGLBoolean eglQueryContext (EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglQueryContext(dpy, ctx, attribute, value);
}

EGLBoolean eglWaitGL (void)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglWaitGL();
}

EGLBoolean eglWaitNative (EGLint engine)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglWaitNative(engine);
}

EGLBoolean eglSwapBuffers (EGLDisplay dpy, EGLSurface draw)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglSwapBuffers(dpy, draw);
}

EGLBoolean eglCopyBuffers (EGLDisplay dpy, EGLSurface surface, NativePixmapType target)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglCopyBuffers(dpy, surface, target);
}

/* extensions */
EGLImageKHR eglCreateImageKHR(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list)
{
    CHECK_GLOBAL_MOCK(EGLImageKHR)
    return global_egl_mock->eglCreateImageKHR ( dpy, ctx, target, buffer, attrib_list);
}

EGLBoolean eglDestroyImageKHR (EGLDisplay dpy, EGLImageKHR image)
{
    CHECK_GLOBAL_MOCK(EGLBoolean)
    return global_egl_mock->eglDestroyImageKHR ( dpy, image);
}

