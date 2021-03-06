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
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#include <mir_test_doubles/mock_surface_factory.h>

#include <mir/shell/organising_surface_factory.h>
#include <mir/shell/placement_strategy.h>
#include <mir/frontend/surface_creation_parameters.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace mf = mir::frontend;
namespace msh = mir::shell;
namespace geom = mir::geometry;
namespace mtd = mir::test::doubles;

namespace
{

struct MockPlacementStrategy : public msh::PlacementStrategy
{
    MOCK_METHOD1(place, mf::SurfaceCreationParameters(mf::SurfaceCreationParameters const&));
};

struct OrganisingSurfaceFactorySetup : public testing::Test
{
    void SetUp()
    {
        using namespace ::testing;

        underlying_surface_factory = std::make_shared<mtd::MockSurfaceFactory>();
        ON_CALL(*underlying_surface_factory, create_surface(_)).WillByDefault(Return(null_surface));

        placement_strategy = std::make_shared<MockPlacementStrategy>();
    }
    std::shared_ptr<msh::Surface> null_surface;
    std::shared_ptr<mtd::MockSurfaceFactory> underlying_surface_factory;
    std::shared_ptr<MockPlacementStrategy> placement_strategy;
};

} // namespace

TEST_F(OrganisingSurfaceFactorySetup, offers_create_surface_parameters_to_placement_strategy)
{
    using namespace ::testing;

    msh::OrganisingSurfaceFactory factory(underlying_surface_factory, placement_strategy);

    EXPECT_CALL(*underlying_surface_factory, create_surface(_)).Times(1);

    auto params = mf::a_surface();
    EXPECT_CALL(*placement_strategy, place(Ref(params))).Times(1)
        .WillOnce(Return(mf::a_surface()));

    factory.create_surface(params);
}

TEST_F(OrganisingSurfaceFactorySetup, forwards_create_surface_parameters_from_placement_strategy_to_underlying_factory)
{
    using namespace ::testing;

    msh::OrganisingSurfaceFactory factory(underlying_surface_factory, placement_strategy);

    auto params = mf::a_surface();
    auto placed_params = params;
    placed_params.size.width = geom::Width{100};

    EXPECT_CALL(*placement_strategy, place(Ref(params))).Times(1)
        .WillOnce(Return(placed_params));
    EXPECT_CALL(*underlying_surface_factory, create_surface(placed_params));

    factory.create_surface(params);
}

