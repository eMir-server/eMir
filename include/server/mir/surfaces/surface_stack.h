/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */

#ifndef MIR_SURFACES_SURFACESTACK_H_
#define MIR_SURFACES_SURFACESTACK_H_

#include "surface_stack_model.h"
#include "mir/compositor/render_view.h"

#include <memory>
#include <vector>
#include <mutex>

namespace mir
{
namespace compositor
{
class RenderableCollection;
class FilterForRenderables;
class OperatorForRenderables;
}

namespace shell
{
class SurfaceCreationParameters;
}

namespace surfaces
{
class BufferBundleFactory;
class Surface;

class SurfaceStack : public compositor::RenderView, public SurfaceStackModel
{
public:
    explicit SurfaceStack(std::shared_ptr<BufferBundleFactory> const& bb_factory);
    virtual ~SurfaceStack() {}

    // From RenderView
    virtual void for_each_if(compositor::FilterForRenderables &filter, compositor::OperatorForRenderables &renderable_operator);

    // From SurfaceStackModel
    virtual std::weak_ptr<Surface> create_surface(const shell::SurfaceCreationParameters& params);

    virtual void destroy_surface(std::weak_ptr<Surface> const& surface);

    virtual void raise_to_top(std::weak_ptr<Surface> surface);

private:
    SurfaceStack(const SurfaceStack&) = delete;
    SurfaceStack& operator=(const SurfaceStack&) = delete;
    std::mutex guard;
    std::shared_ptr<BufferBundleFactory> const buffer_bundle_factory;
    std::vector<std::shared_ptr<Surface>> surfaces;
};

}
}

#endif /* MIR_SURFACES_SURFACESTACK_H_ */