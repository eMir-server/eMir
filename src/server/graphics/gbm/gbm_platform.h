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
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#ifndef MIR_GRAPHICS_GBM_GBM_PLATFORM_H_
#define MIR_GRAPHICS_GBM_GBM_PLATFORM_H_

#include "mir/graphics/platform.h"
#include "mir/graphics/drm_authenticator.h"
#include "gbm_display_helpers.h"

namespace mir
{
namespace graphics
{
namespace gbm
{

class GBMPlatform : public Platform,
                    public DRMAuthenticator,
                    public std::enable_shared_from_this<GBMPlatform>
{
public:
    explicit GBMPlatform(std::shared_ptr<DisplayReport> const& reporter);

    /* From Platform */
    std::shared_ptr<compositor::GraphicBufferAllocator> create_buffer_allocator(
            const std::shared_ptr<BufferInitializer>& buffer_initializer);
    std::shared_ptr<Display> create_display();
    std::shared_ptr<PlatformIPCPackage> get_ipc_package();

    /* From DRMAuthenticator */
    void drm_auth_magic(drm_magic_t magic);

    helpers::DRMHelper drm;
    helpers::GBMHelper gbm;

    std::shared_ptr<DisplayReport> listener;
};

}
}
}
#endif /* MIR_GRAPHICS_GBM_GBM_PLATFORM_H_ */

