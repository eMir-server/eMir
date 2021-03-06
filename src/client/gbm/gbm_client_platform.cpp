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
 * Authored by: Kevin DuBois<kevin.dubois@canonical.com>
 */

#include "mir_toolkit/mir_client_library.h"
#include "gbm_client_platform.h"
#include "gbm_client_buffer_factory.h"
#include "mesa_native_display_container.h"
#include "drm_fd_handler.h"
#include "../mir_connection.h"
#include "../client_buffer_factory.h"
#include "../native_client_platform_factory.h"

#include <xf86drm.h>
#include <sys/mman.h>

namespace mcl=mir::client;
namespace mclg=mir::client::gbm;
namespace geom=mir::geometry;

namespace
{

class RealDRMFDHandler : public mclg::DRMFDHandler
{
public:
    RealDRMFDHandler(int drm_fd) : drm_fd{drm_fd}
    {
    }

    int ioctl(unsigned long request, void* arg)
    {
        return drmIoctl(drm_fd, request, arg);
    }

    void* map(size_t size, off_t offset)
    {
        return mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                    drm_fd, offset);
    }

    void unmap(void* addr, size_t size)
    {
        munmap(addr, size);
    }

private:
    int drm_fd;
};

struct NativeDisplayDeleter
{
    NativeDisplayDeleter(mcl::EGLNativeDisplayContainer& container)
    : container(container)
    {
    }
    void operator() (EGLNativeDisplayType* p)
    {
        auto display = *(reinterpret_cast<MirEGLNativeDisplayType*>(p));
        container.release(display);
        delete p;
    }

    mcl::EGLNativeDisplayContainer& container;
};

}

std::shared_ptr<mcl::ClientPlatform>
mcl::NativeClientPlatformFactory::create_client_platform(mcl::ClientContext* context)
{
    mir_toolkit::MirPlatformPackage platform_package;

    memset(&platform_package, 0, sizeof(platform_package));
    context->populate(platform_package);

    int drm_fd = -1;

    if (platform_package.fd_items > 0)
        drm_fd = platform_package.fd[0];

    auto drm_fd_handler = std::make_shared<RealDRMFDHandler>(drm_fd);
    return std::make_shared<mclg::GBMClientPlatform>(context, drm_fd_handler, mcl::EGLNativeDisplayContainer::instance());
}

mclg::GBMClientPlatform::GBMClientPlatform(
        ClientContext* const context,
        std::shared_ptr<DRMFDHandler> const& drm_fd_handler,
        mcl::EGLNativeDisplayContainer& display_container)
    : context{context},
      drm_fd_handler{drm_fd_handler},
      display_container(display_container)
{
}

std::shared_ptr<mcl::ClientBufferFactory> mclg::GBMClientPlatform::create_buffer_factory()
{
    return std::make_shared<mclg::GBMClientBufferFactory>(drm_fd_handler);
}

std::shared_ptr<EGLNativeWindowType> mclg::GBMClientPlatform::create_egl_native_window(ClientSurface* client_surface)
{
    auto window_type = std::make_shared<EGLNativeWindowType>();
    *window_type = reinterpret_cast<EGLNativeWindowType>(client_surface);
    return window_type;
}

std::shared_ptr<EGLNativeDisplayType> mclg::GBMClientPlatform::create_egl_native_display()
{
    MirEGLNativeDisplayType *mir_native_display = new MirEGLNativeDisplayType;
    *mir_native_display = display_container.create(context->mir_connection());
    auto egl_native_display = reinterpret_cast<EGLNativeDisplayType*>(mir_native_display);

    return std::shared_ptr<EGLNativeDisplayType>(egl_native_display, NativeDisplayDeleter(display_container));
}
