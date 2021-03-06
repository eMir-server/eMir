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
 * Authored By: Robert Carr <racarr@canonical.com>
 */

#include "mir/shell/application_session.h"
#include "mir/compositor/buffer.h"
#include "mir/frontend/surface_creation_parameters.h"
#include "mir_test/fake_shared.h"
#include "mir_test_doubles/mock_surface_factory.h"
#include "mir_test_doubles/mock_surface.h"
#include "mir_test_doubles/stub_surface_builder.h"
#include "mir_test_doubles/stub_surface.h"

#include "src/server/shell/surface.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mc = mir::compositor;
namespace mf = mir::frontend;
namespace msh = mir::shell;
namespace ms = mir::surfaces;
namespace mi = mir::input;
namespace mt = mir::test;
namespace mtd = mir::test::doubles;

TEST(ApplicationSession, create_and_destroy_surface)
{
    using namespace ::testing;

    mtd::StubSurfaceBuilder surface_builder;
    auto const mock_surface = std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder));

    mtd::MockSurfaceFactory surface_factory;
    ON_CALL(surface_factory, create_surface(_)).WillByDefault(Return(mock_surface));

    EXPECT_CALL(surface_factory, create_surface(_));
    EXPECT_CALL(*mock_surface, destroy());

    msh::ApplicationSession session(mt::fake_shared(surface_factory), "Foo");

    mf::SurfaceCreationParameters params;
    auto surf = session.create_surface(params);

    session.destroy_surface(surf);
}

TEST(ApplicationSession, default_surface_is_first_surface)
{
    using namespace ::testing;

    mtd::MockSurfaceFactory surface_factory;
    mtd::StubSurfaceBuilder surface_builder;
    {
        InSequence seq;
        EXPECT_CALL(surface_factory, create_surface(_)).Times(1)
            .WillOnce(Return(std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder))));
        EXPECT_CALL(surface_factory, create_surface(_)).Times(1)
            .WillOnce(Return(std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder))));
        EXPECT_CALL(surface_factory, create_surface(_)).Times(1)
            .WillOnce(Return(std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder))));
    }

    msh::ApplicationSession app_session(mt::fake_shared(surface_factory), "Foo");

    mf::SurfaceCreationParameters params;
    auto id1 = app_session.create_surface(params);
    auto id2 = app_session.create_surface(params);
    auto id3 = app_session.create_surface(params);

    auto default_surf = app_session.default_surface();
    EXPECT_EQ(app_session.get_surface(id1), default_surf);
    app_session.destroy_surface(id1);

    default_surf = app_session.default_surface();
    EXPECT_EQ(app_session.get_surface(id2), default_surf);
    app_session.destroy_surface(id2);

    default_surf = app_session.default_surface();
    EXPECT_EQ(app_session.get_surface(id3), default_surf);
    app_session.destroy_surface(id3);
}

TEST(ApplicationSession, session_visbility_propagates_to_surfaces)
{
    using namespace ::testing;

    mtd::StubSurfaceBuilder surface_builder;
    auto const mock_surface = std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder));

    mtd::MockSurfaceFactory surface_factory;
    ON_CALL(surface_factory, create_surface(_)).WillByDefault(Return(mock_surface));

    msh::ApplicationSession app_session(mt::fake_shared(surface_factory), "Foo");

    EXPECT_CALL(surface_factory, create_surface(_));

    {
        InSequence seq;
        EXPECT_CALL(*mock_surface, hide()).Times(1);
        EXPECT_CALL(*mock_surface, show()).Times(1);
        EXPECT_CALL(*mock_surface, destroy()).Times(1);
    }

    mf::SurfaceCreationParameters params;
    auto surf = app_session.create_surface(params);

    app_session.hide();
    app_session.show();

    app_session.destroy_surface(surf);
}

TEST(Session, get_invalid_surface_throw_behavior)
{
    using namespace ::testing;

    mtd::MockSurfaceFactory surface_factory;
    msh::ApplicationSession app_session(mt::fake_shared(surface_factory), "Foo");
    mf::SurfaceId invalid_surface_id(1);

    EXPECT_THROW({
            app_session.get_surface(invalid_surface_id);
    }, std::runtime_error);
}

TEST(Session, destroy_invalid_surface_throw_behavior)
{
    using namespace ::testing;

    mtd::MockSurfaceFactory surface_factory;
    msh::ApplicationSession app_session(mt::fake_shared(surface_factory), "Foo");
    mf::SurfaceId invalid_surface_id(1);

    EXPECT_THROW({
            app_session.destroy_surface(invalid_surface_id);
    }, std::runtime_error);
}
