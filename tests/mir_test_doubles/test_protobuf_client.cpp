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

#include "mir_test/test_protobuf_client.h"

#include <thread>

namespace mtd = mir::test::doubles;

mir::test::TestProtobufClient::TestProtobufClient(
    std::string socket_file,
    int timeout_ms) :
    logger(std::make_shared<mtd::MockLogger>()),
    channel(mir::client::make_rpc_channel(socket_file, logger)),
    display_server(channel.get(), ::google::protobuf::Service::STUB_DOESNT_OWN_CHANNEL),
    maxwait(timeout_ms),
    connect_done_called(false),
    create_surface_called(false),
    next_buffer_called(false),
    release_surface_called(false),
    disconnect_done_called(false),
    tfd_done_called(false),
    connect_done_count(0),
    create_surface_done_count(0),
    disconnect_done_count(0)
{
    surface_parameters.set_width(640);
    surface_parameters.set_height(480);
    surface_parameters.set_pixel_format(0);
    surface_parameters.set_buffer_usage(0);

    ON_CALL(*this, connect_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_connect_done));
    ON_CALL(*this, create_surface_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_create_surface_done));
    ON_CALL(*this, next_buffer_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_next_buffer_done));
    ON_CALL(*this, release_surface_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_release_surface_done));
    ON_CALL(*this, disconnect_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_disconnect_done));
    ON_CALL(*this, drm_auth_magic_done()).WillByDefault(testing::Invoke(this, &TestProtobufClient::on_drm_auth_magic_done));
}

void mir::test::TestProtobufClient::on_connect_done()
{
    connect_done_called.store(true);

    auto old = connect_done_count.load();

    while (!connect_done_count.compare_exchange_weak(old, old+1));
}

void mir::test::TestProtobufClient::on_create_surface_done()
{
    create_surface_called.store(true);

    auto old = create_surface_done_count.load();

    while (!create_surface_done_count.compare_exchange_weak(old, old+1));
}

void mir::test::TestProtobufClient::on_next_buffer_done()
{
    next_buffer_called.store(true);
}

void mir::test::TestProtobufClient::on_release_surface_done()
{
    release_surface_called.store(true);
}

void mir::test::TestProtobufClient::on_disconnect_done()
{
    disconnect_done_called.store(true);

    auto old = disconnect_done_count.load();

    while (!disconnect_done_count.compare_exchange_weak(old, old+1));
}

void mir::test::TestProtobufClient::on_drm_auth_magic_done()
{
    drm_auth_magic_done_called.store(true);
}

void mir::test::TestProtobufClient::wait_for_connect_done()
{
    for (int i = 0; !connect_done_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }

    connect_done_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_create_surface()
{
    for (int i = 0; !create_surface_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }
    create_surface_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_next_buffer()
{
    for (int i = 0; !next_buffer_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }
    next_buffer_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_release_surface()
{
    for (int i = 0; !release_surface_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }
    release_surface_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_disconnect_done()
{
    for (int i = 0; !disconnect_done_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }
    disconnect_done_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_drm_auth_magic_done()
{
    for (int i = 0; !drm_auth_magic_done_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::this_thread::yield();
    }
    drm_auth_magic_done_called.store(false);
}

void mir::test::TestProtobufClient::wait_for_surface_count(int count)
{
    for (int i = 0; count != create_surface_done_count.load() && i < 10000; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::this_thread::yield();
    }
}

void mir::test::TestProtobufClient::wait_for_disconnect_count(int count)
{
    for (int i = 0; count != disconnect_done_count.load() && i < 10000; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::this_thread::yield();
    }
}

void mir::test::TestProtobufClient::tfd_done()
{
    tfd_done_called.store(true);
}

void mir::test::TestProtobufClient::wait_for_tfd_done()
{
    for (int i = 0; !tfd_done_called.load() && i < maxwait; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    tfd_done_called.store(false);
}
