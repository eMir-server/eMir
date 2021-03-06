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
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#include "mir/surfaces/buffer_bundle.h"
#include "mir/compositor/buffer_ipc_package.h"
#include "mir/compositor/graphic_buffer_allocator.h"
#include "mir/frontend/session_mediator_report.h"
#include "mir/frontend/session_mediator.h"
#include "mir/frontend/resource_cache.h"
#include "mir/shell/application_session.h"
#include "mir/graphics/display.h"
#include "mir/graphics/platform.h"
#include "mir/graphics/platform_ipc_package.h"
#include "mir/surfaces/surface.h"
#include "mir_test_doubles/null_display.h"
#include "mir_test_doubles/mock_shell.h"
#include "mir_test_doubles/mock_surface.h"
#include "mir_test_doubles/stub_buffer.h"
#include "mir_test_doubles/stub_session.h"
#include "mir_test_doubles/stub_surface_builder.h"
#include "mir_test/fake_shared.h"

#include "src/server/shell/surface.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdexcept>

namespace mf = mir::frontend;
namespace mg = mir::graphics;
namespace mc = mir::compositor;
namespace ms = mir::surfaces;
namespace geom = mir::geometry;
namespace mp = mir::protobuf;
namespace msh = mir::shell;
namespace mt = mir::test;
namespace mtd = mt::doubles;

namespace
{

class StubbedSession : public mtd::StubSession
{
public:
    StubbedSession()
    {
        using namespace ::testing;

        mock_surface = std::make_shared<mtd::MockSurface>(mt::fake_shared(surface_builder));

        EXPECT_CALL(*mock_surface, size()).Times(AnyNumber()).WillRepeatedly(Return(geom::Size()));
        EXPECT_CALL(*mock_surface, pixel_format()).Times(AnyNumber()).WillRepeatedly(Return(geom::PixelFormat()));
        EXPECT_CALL(*mock_surface, client_buffer()).Times(AnyNumber()).WillRepeatedly(Return(mt::fake_shared(stub_buffer)));
        EXPECT_CALL(*mock_surface, advance_client_buffer()).Times(AnyNumber());

        EXPECT_CALL(*mock_surface, supports_input()).Times(AnyNumber()).WillRepeatedly(Return(true));
        EXPECT_CALL(*mock_surface, client_input_fd()).Times(AnyNumber()).WillRepeatedly(Return(testing_client_input_fd));
    }

    std::shared_ptr<mf::Surface> get_surface(mf::SurfaceId /* surface */) const
    {
        return mock_surface;
    }

    mtd::StubSurfaceBuilder surface_builder;
    std::shared_ptr<mtd::MockSurface> mock_surface;
    mtd::StubBuffer stub_buffer;
    static int const testing_client_input_fd;
};

int const StubbedSession::testing_client_input_fd{11};

class MockGraphicBufferAllocator : public mc::GraphicBufferAllocator
{
public:
    MockGraphicBufferAllocator()
    {
        ON_CALL(*this, supported_pixel_formats())
            .WillByDefault(testing::Return(std::vector<geom::PixelFormat>()));
    }

    std::shared_ptr<mc::Buffer> alloc_buffer(mc::BufferProperties const&)
    {
        return std::shared_ptr<mc::Buffer>();
    }

    MOCK_METHOD0(supported_pixel_formats, std::vector<geom::PixelFormat>());
};

class StubPlatform : public mg::Platform
{
 public:
    std::shared_ptr<mc::GraphicBufferAllocator> create_buffer_allocator(
            const std::shared_ptr<mg::BufferInitializer>& /*buffer_initializer*/)
    {
        return std::shared_ptr<mc::GraphicBufferAllocator>();
    }

    std::shared_ptr<mg::Display> create_display()
    {
        return std::make_shared<mtd::NullDisplay>();
    }

    std::shared_ptr<mg::PlatformIPCPackage> get_ipc_package()
    {
        return std::make_shared<mg::PlatformIPCPackage>();
    }
};

}

struct SessionMediatorTest : public ::testing::Test
{
    SessionMediatorTest()
        : shell{std::make_shared<testing::NiceMock<mtd::MockShell>>()},
          graphics_platform{std::make_shared<StubPlatform>()},
          graphics_display{std::make_shared<mtd::NullDisplay>()},
          buffer_allocator{std::make_shared<testing::NiceMock<MockGraphicBufferAllocator>>()},
          report{std::make_shared<mf::NullSessionMediatorReport>()},
          resource_cache{std::make_shared<mf::ResourceCache>()},
          mediator{shell, graphics_platform, graphics_display,
                   buffer_allocator, report, resource_cache},
          stubbed_session{std::make_shared<StubbedSession>()},
          null_callback{google::protobuf::NewPermanentCallback(google::protobuf::DoNothing)}
    {
        using namespace ::testing;

        ON_CALL(*shell, open_session(_)).WillByDefault(Return(stubbed_session));
        ON_CALL(*shell, create_surface_for(_, _)).WillByDefault(Return(mf::SurfaceId{1}));
    }

    std::shared_ptr<testing::NiceMock<mtd::MockShell>> const shell;
    std::shared_ptr<mg::Platform> const graphics_platform;
    std::shared_ptr<mg::Display> const graphics_display;
    std::shared_ptr<testing::NiceMock<MockGraphicBufferAllocator>> const buffer_allocator;
    std::shared_ptr<mf::SessionMediatorReport> const report;
    std::shared_ptr<mf::ResourceCache> const resource_cache;
    mf::SessionMediator mediator;
    std::shared_ptr<StubbedSession> const stubbed_session;

    std::unique_ptr<google::protobuf::Closure> null_callback;
};


TEST_F(SessionMediatorTest, disconnect_releases_session)
{
    using namespace ::testing;

    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    EXPECT_CALL(*shell, close_session(_)).Times(1);

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());
    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
}

TEST_F(SessionMediatorTest, calling_methods_before_connect_throws)
{
    EXPECT_THROW({
        mp::SurfaceParameters request;
        mp::Surface response;

        mediator.create_surface(nullptr, &request, &response, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::SurfaceId request;
        mp::Buffer response;

        mediator.next_buffer(nullptr, &request, &response, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::SurfaceId request;

        mediator.release_surface(nullptr, &request, nullptr, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::DRMMagic request;
        mp::DRMAuthMagicStatus response;

        mediator.drm_auth_magic(nullptr, &request, &response, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
    }, std::logic_error);
}

TEST_F(SessionMediatorTest, calling_methods_after_connect_works)
{
    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    {
        mp::SurfaceParameters request;
        mp::Surface response;

        mediator.create_surface(nullptr, &request, &response, null_callback.get());
    }
    {
        mp::SurfaceId request;
        mp::Buffer response;

        mediator.next_buffer(nullptr, &request, &response, null_callback.get());
    }
    {
        mp::SurfaceId request;

        mediator.release_surface(nullptr, &request, nullptr, null_callback.get());
    }

    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
}

TEST_F(SessionMediatorTest, calling_methods_after_disconnect_throws)
{
    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());

    EXPECT_THROW({
        mp::SurfaceParameters surface_parameters;
        mp::Surface surface;

        mediator.create_surface(nullptr, &surface_parameters, &surface, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::SurfaceId request;
        mp::Buffer response;

        mediator.next_buffer(nullptr, &request, &response, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::SurfaceId request;

        mediator.release_surface(nullptr, &request, nullptr, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mp::DRMMagic request;
        mp::DRMAuthMagicStatus response;

        mediator.drm_auth_magic(nullptr, &request, &response, null_callback.get());
    }, std::logic_error);

    EXPECT_THROW({
        mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
    }, std::logic_error);
}

TEST_F(SessionMediatorTest, can_reconnect_after_disconnect)
{
    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());
}

TEST_F(SessionMediatorTest, connect_queries_supported_pixel_formats)
{
    using namespace testing;

    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    std::vector<geom::PixelFormat> const pixel_formats{
        geom::PixelFormat::bgr_888,
        geom::PixelFormat::abgr_8888,
        geom::PixelFormat::xbgr_8888
    };

    EXPECT_CALL(*buffer_allocator, supported_pixel_formats())
        .WillOnce(Return(pixel_formats));

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    auto info = connection.display_info();

    ASSERT_EQ(pixel_formats.size(), static_cast<size_t>(info.supported_pixel_format_size()));

    for (size_t i = 0; i < pixel_formats.size(); ++i)
    {
        EXPECT_EQ(pixel_formats[i], static_cast<geom::PixelFormat>(info.supported_pixel_format(i)))
            << "i = " << i;
    }
}

TEST_F(SessionMediatorTest, creating_surface_packs_response_with_input_fds)
{
    mp::ConnectParameters connect_parameters;
    mp::Connection connection;

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    {
        mp::SurfaceParameters request;
        mp::Surface response;

        mediator.create_surface(nullptr, &request, &response, null_callback.get());
        EXPECT_EQ(StubbedSession::testing_client_input_fd, response.fd(0));
    }

    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
}

TEST_F(SessionMediatorTest, no_input_channel_is_nonfatal)
{
    mp::ConnectParameters connect_parameters;
    mp::Connection connection;
    EXPECT_CALL(*stubbed_session->mock_surface, supports_input())
        .Times(1)
        .WillOnce(testing::Return(false));
    EXPECT_CALL(*stubbed_session->mock_surface, client_input_fd())
        .Times(0);

    mediator.connect(nullptr, &connect_parameters, &connection, null_callback.get());

    {
        mp::SurfaceParameters request;
        mp::Surface response;

        mediator.create_surface(nullptr, &request, &response, null_callback.get());
    }

    mediator.disconnect(nullptr, nullptr, nullptr, null_callback.get());
}
