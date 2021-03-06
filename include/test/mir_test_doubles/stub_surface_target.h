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
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#ifndef MIR_TEST_DOUBLES_STUB_SURFACE_TARGET_H_
#define MIR_TEST_DOUBLES_STUB_SURFACE_TARGET_H_

#include "mir/input/surface_target.h"

namespace mir
{
namespace test
{
namespace doubles
{

struct StubSurfaceTarget : public input::SurfaceTarget
{
    StubSurfaceTarget(int fd)
      : input_fd(fd)
    {
    }

    int server_input_fd() const override
    {
        return input_fd;
    }
    geometry::Size size() const override
    {
        return geometry::Size();
    }
    std::string name() const override
    {
        return std::string();
    }

    int input_fd;
};

}
}
} // namespace mir

#endif // MIR_TEST_DOUBLES_STUB_SURFACE_TARGET_H_

