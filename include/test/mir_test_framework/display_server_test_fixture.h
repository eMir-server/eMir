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
 * Authored by: Thomas Guest <thomas.guest@canonical.com>
 */

#ifndef MIR_DISPLAY_SERVER_TEST_FIXTURE
#define MIR_DISPLAY_SERVER_TEST_FIXTURE

#include "mir_test_framework/process.h"

#include "mir_test_framework/testing_process_manager.h"
#include <gtest/gtest.h>

#include <memory>

namespace mir_test_framework
{
using namespace ::mir;

// The test fixture sets up and tears down a display server for use
// in display server test cases.
class DefaultDisplayServerTestFixture : public testing::Test
{
public:
    DefaultDisplayServerTestFixture();
    ~DefaultDisplayServerTestFixture();

    static void SetUpTestCase();
    static void TearDownTestCase();

    void launch_client_process(TestingClientConfiguration& config);

private:
    static TestingProcessManager process_manager;

    virtual void TearDown();
    static std::shared_ptr<mir::frontend::Communicator> make_communicator();
    static std::shared_ptr<mir::graphics::Renderer> the_renderer();
    static std::shared_ptr<mir::compositor::BufferAllocationStrategy> the_buffer_allocation_strategy();
};

// The test fixture sets up and tears down a display server for use
// in display server tests.
class BespokeDisplayServerTestFixture : public testing::Test
{
public:
    BespokeDisplayServerTestFixture();
    ~BespokeDisplayServerTestFixture();

    void launch_server_process(TestingServerConfiguration& config);

    void launch_client_process(TestingClientConfiguration& config);

    bool shutdown_server_process();
    void kill_client_processes();

protected:
    virtual void SetUp();
    virtual void TearDown();

private:
    TestingProcessManager process_manager;
};

}

using mir_test_framework::DefaultDisplayServerTestFixture;
using mir_test_framework::BespokeDisplayServerTestFixture;
using mir_test_framework::TestingClientConfiguration;
using mir_test_framework::TestingServerConfiguration;

#endif // MIR_DISPLAY_SERVER_TEST_FIXTURE
