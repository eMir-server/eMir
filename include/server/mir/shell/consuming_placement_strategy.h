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

#ifndef MIR_SHELL_CONSUMING_PLACEMENT_STRATEGY_H_
#define MIR_SHELL_CONSUMING_PLACEMENT_STRATEGY_H_

#include "mir/shell/placement_strategy.h"

#include <memory>

namespace mir
{
namespace graphics
{
class ViewableArea;
}
namespace shell
{

class ConsumingPlacementStrategy : public PlacementStrategy
{
public:
    explicit ConsumingPlacementStrategy(std::shared_ptr<graphics::ViewableArea> const& display_area);
    virtual ~ConsumingPlacementStrategy() {}

    virtual frontend::SurfaceCreationParameters place(frontend::SurfaceCreationParameters const& request_parameters);

protected:
    ConsumingPlacementStrategy(ConsumingPlacementStrategy const&) = delete;
    ConsumingPlacementStrategy& operator=(ConsumingPlacementStrategy const&) = delete;

private:
    std::shared_ptr<graphics::ViewableArea> const display_area;
};

}
} // namespace mir

#endif // MIR_SHELL_CONSUMING_PLACEMENT_STRATEGY_H_
