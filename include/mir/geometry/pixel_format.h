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
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */


#ifndef MIR_COMPOSITOR_PIXEL_FORMAT_H_
#define MIR_COMPOSITOR_PIXEL_FORMAT_H_

#include <cstdint>
#include <cstddef>

namespace mir
{
namespace geometry
{
enum class PixelFormat : uint32_t
{
    rgba_8888,
    rgb_888,
    pixel_format_invalid
};

class PixelOperation
{
public:
    PixelOperation();

    size_t bytes_per_pixel(PixelFormat pf)
    {
        switch(pf)
        {
            case PixelFormat::rgba_8888:
                return 4;
            case PixelFormat::rgb_888:
                return 3;
            default:
                return 0;
        }
    }
};

}
}

#endif /* MIR_COMPOSITOR_PIXEL_FORMAT_H_ */