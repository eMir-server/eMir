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
 *              Alan Griffiths <alan@octopull.co.uk>
 */

#include "mir/frontend/communicator.h"
#include "mir/frontend/resource_cache.h"

#include "mir_protobuf.pb.h"

#include "mir_test_doubles/stub_ipc_factory.h"
#include "mir_test_doubles/mock_logger.h"
#include "mir_test/stub_server_tool.h"
#include "mir_test/test_protobuf_client.h"
#include "mir_test/test_protobuf_server.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <stdexcept>
#include <memory>
#include <string>

namespace mf = mir::frontend;
namespace mt = mir::test;

namespace mir
{
struct ProtobufCommunicator : public ::testing::Test
{
    static void SetUpTestCase()
    {
        stub_server_tool = std::make_shared<mt::StubServerTool>();
        stub_server = std::make_shared<mt::TestProtobufServer>("./test_socket", stub_server_tool);

        stub_server->comm->start();
    }

    void SetUp()
    {
        client = std::make_shared<mt::TestProtobufClient>("./test_socket", 100);
        client->connect_parameters.set_application_name(__PRETTY_FUNCTION__);
    }

    void TearDown()
    {
        client.reset();
    }

    static void TearDownTestCase()
    {
        stub_server.reset();
        stub_server_tool.reset();
    }

    std::shared_ptr<mt::TestProtobufClient> client;
    static std::shared_ptr<mt::StubServerTool> stub_server_tool;
private:
    static std::shared_ptr<mt::TestProtobufServer> stub_server;
};

std::shared_ptr<mt::StubServerTool> ProtobufCommunicator::stub_server_tool;
std::shared_ptr<mt::TestProtobufServer> ProtobufCommunicator::stub_server;

TEST_F(ProtobufCommunicator, create_surface_results_in_a_callback)
{
    EXPECT_CALL(*client, create_surface_done()).Times(1);

    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();
}

TEST_F(ProtobufCommunicator, connection_sets_app_name)
{
    EXPECT_CALL(*client, connect_done()).Times(1);

    client->connect_parameters.set_application_name(__PRETTY_FUNCTION__);

    client->display_server.connect(
        0,
        &client->connect_parameters,
        &client->connection,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::connect_done));

    client->wait_for_connect_done();

    EXPECT_EQ(__PRETTY_FUNCTION__, stub_server_tool->app_name);
}

TEST_F(ProtobufCommunicator, create_surface_sets_surface_name)
{
    EXPECT_CALL(*client, connect_done()).Times(1);
    EXPECT_CALL(*client, create_surface_done()).Times(1);

    client->connect_parameters.set_application_name(__PRETTY_FUNCTION__);

    client->display_server.connect(
        0,
        &client->connect_parameters,
        &client->connection,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::connect_done));

    client->wait_for_connect_done();

    client->surface_parameters.set_surface_name(__PRETTY_FUNCTION__);

    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();

    EXPECT_EQ(__PRETTY_FUNCTION__, stub_server_tool->surface_name);
}


TEST_F(ProtobufCommunicator,
        create_surface_results_in_a_surface_being_created)
{
    EXPECT_CALL(*client, create_surface_done()).Times(1);

    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();
}

TEST_F(ProtobufCommunicator,
       double_disconnection_attempt_has_no_effect)
{
    EXPECT_CALL(*client, create_surface_done()).Times(1);
    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();

    EXPECT_CALL(*client, disconnect_done()).Times(1);
    client->display_server.disconnect(
        0,
        &client->ignored,
        &client->ignored,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::disconnect_done));

    client->wait_for_disconnect_done();

    // socket based & binder based rpc fail differently, but either way
    // the test ensures that nothing horrible happens.
    EXPECT_CALL(*client->logger, error()).Times(testing::AtMost(3));
    EXPECT_CALL(*client, disconnect_done()).Times(testing::AtMost(1));

    // We don't know if this will be called, so it can't auto destruct
    std::unique_ptr<google::protobuf::Closure> new_callback(google::protobuf::NewPermanentCallback(client.get(), &mt::TestProtobufClient::disconnect_done));
    client->display_server.disconnect(0, &client->ignored, &client->ignored, new_callback.get());
    client->wait_for_disconnect_done();
}

TEST_F(ProtobufCommunicator,
       getting_and_advancing_buffers)
{
    EXPECT_CALL(*client, create_surface_done()).Times(testing::AtLeast(0));
    EXPECT_CALL(*client, disconnect_done()).Times(testing::AtLeast(0));

    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();

    EXPECT_TRUE(client->surface.has_buffer());
    EXPECT_CALL(*client, next_buffer_done()).Times(8);

    for (int i = 0; i != 8; ++i)
    {
        client->display_server.next_buffer(
            0,
            &client->surface.id(),
            client->surface.mutable_buffer(),
            google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::next_buffer_done));

        client->wait_for_next_buffer();
        EXPECT_TRUE(client->surface.has_buffer());
    }

    client->display_server.disconnect(
        0,
        &client->ignored,
        &client->ignored,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::disconnect_done));

    client->wait_for_disconnect_done();
}

TEST_F(ProtobufCommunicator,
       connect_create_surface_then_disconnect_a_session)
{
    EXPECT_CALL(*client, create_surface_done()).Times(1);
    client->display_server.create_surface(
        0,
        &client->surface_parameters,
        &client->surface,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::create_surface_done));

    client->wait_for_create_surface();

    EXPECT_CALL(*client, disconnect_done()).Times(1);
    client->display_server.disconnect(
        0,
        &client->ignored,
        &client->ignored,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::disconnect_done));

    client->wait_for_disconnect_done();
}

TEST_F(ProtobufCommunicator, drm_auth_magic_is_processed_by_the_server)
{
    mir::protobuf::DRMMagic magic;
    mir::protobuf::DRMAuthMagicStatus status;
    magic.set_magic(0x10111213);

    EXPECT_CALL(*client, drm_auth_magic_done()).Times(1);

    client->display_server.drm_auth_magic(
        0,
        &magic,
        &status,
        google::protobuf::NewCallback(client.get(), &mt::TestProtobufClient::drm_auth_magic_done));

    client->wait_for_drm_auth_magic_done();

    EXPECT_EQ(magic.magic(), stub_server_tool->drm_magic);
}
}
