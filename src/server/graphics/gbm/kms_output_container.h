/*
 * Copyright © 2013 Canonical Ltd.
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

#ifndef MIR_GRAPHICS_GBM_KMS_OUTPUT_CONTAINER_H_
#define MIR_GRAPHICS_GBM_KMS_OUTPUT_CONTAINER_H_

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace mir
{
namespace graphics
{
namespace gbm
{

class KMSOutput;
class PageFlipper;

class KMSOutputContainer
{
public:
    KMSOutputContainer(int drm_fd, std::shared_ptr<PageFlipper> const& page_flipper);

    std::shared_ptr<KMSOutput> get_kms_output_for(uint32_t connector_id);

private:
    KMSOutputContainer(KMSOutputContainer const&) = delete;
    KMSOutputContainer& operator=(KMSOutputContainer const&) = delete;

    int const drm_fd;
    std::unordered_map<uint32_t,std::shared_ptr<KMSOutput>> outputs;
    std::shared_ptr<PageFlipper> const page_flipper;
};

}
}
}

#endif /* MIR_GRAPHICS_GBM_KMS_OUTPUT_CONTAINER_H_ */
