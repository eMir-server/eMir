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
 * Authored by: Thomas Voss <thomas.voss@canonical.com>
 */

#include "mir/compositor/buffer_swapper.h"
#include "mir/compositor/buffer_allocation_strategy.h"
#include "mir/compositor/buffer_bundle_manager.h"
#include "mir/compositor/graphic_buffer_allocator.h"
#include "mir/compositor/buffer_properties.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mir_test/gmock_fixes.h"
#include "mir_test/fake_shared.h"

namespace mc = mir::compositor;
namespace geom = mir::geometry;
namespace mt = mir::test;

namespace
{

struct MockBufferAllocationStrategy : public mc::BufferAllocationStrategy
{
    MOCK_METHOD2(
        create_swapper,
        std::unique_ptr<mc::BufferSwapper>(mc::BufferProperties&, mc::BufferProperties const&));
};

geom::Size size{geom::Width{1024}, geom::Height{768}};
const geom::PixelFormat pixel_format{geom::PixelFormat::abgr_8888};
const mc::BufferUsage usage{mc::BufferUsage::software};
const mc::BufferProperties buffer_properties{size, pixel_format, usage};

}

TEST(buffer_manager, create_buffer)
{
    using namespace testing;

    MockBufferAllocationStrategy allocation_strategy;

    EXPECT_CALL(allocation_strategy, create_swapper(_,buffer_properties))
        .Times(AtLeast(1));

    mc::BufferBundleManager buffer_bundle_manager(mt::fake_shared(allocation_strategy));

    auto bundle = buffer_bundle_manager.create_buffer_bundle(buffer_properties);

    EXPECT_TRUE(bundle.get());
}
