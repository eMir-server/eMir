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
 * Authored by: Thomas Voss <thomas.voss@canonical.com>
 */

#ifndef MIR_SHELL_SURFACE_FACTORY_H_
#define MIR_SHELL_SURFACE_FACTORY_H_

#include <memory>

namespace mir
{
namespace frontend
{
struct SurfaceCreationParameters;
class Surface;
}

namespace shell
{
class Surface;
class SurfaceFactory
{
public:
    virtual std::shared_ptr<Surface> create_surface(const frontend::SurfaceCreationParameters& params) = 0;

protected:
    virtual ~SurfaceFactory() {}
    SurfaceFactory() = default;
    SurfaceFactory(const SurfaceFactory&) = delete;
    SurfaceFactory& operator=(const SurfaceFactory&) = delete;
};
}
}

#endif // MIR_SHELL_SURFACE_FACTORY_H_
