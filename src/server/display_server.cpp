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
 * Authored by:
 *   Alan Griffiths <alan@octopull.co.uk>
 *   Thomas Voss <thomas.voss@canonical.com>
 */

#include "mir/display_server.h"
#include "mir/server_configuration.h"

#include "mir/compositor/compositor.h"
#include "mir/frontend/shell.h"
#include "mir/frontend/communicator.h"
#include "mir/graphics/display.h"
#include "mir/input/input_manager.h"

#include <mutex>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace mc = mir::compositor;
namespace mf = mir::frontend;
namespace mg = mir::graphics;
namespace mi = mir::input;

struct mir::DisplayServer::Private
{
    Private(ServerConfiguration& config)
        : display{config.the_display()},
          compositor{config.the_compositor()},
          shell{config.the_frontend_shell()},
          communicator{config.the_communicator()},
          input_manager{config.the_input_manager()},
          exit(false)
    {
    }

    std::shared_ptr<mg::Display> display;
    std::shared_ptr<mc::Compositor> compositor;
    std::shared_ptr<frontend::Shell> shell;
    std::shared_ptr<mf::Communicator> communicator;
    std::shared_ptr<mi::InputManager> input_manager;
    std::mutex exit_guard;
    std::condition_variable exit_cv;
    bool exit;
};

mir::DisplayServer::DisplayServer(ServerConfiguration& config) :
    p()
{
    p.reset(new DisplayServer::Private(config));
}

mir::DisplayServer::~DisplayServer()
{
    p->shell->shutdown();
}

void mir::DisplayServer::run()
{
    std::unique_lock<std::mutex> lk(p->exit_guard);

    p->communicator->start();
    p->compositor->start();
    p->input_manager->start();

    while (!p->exit)
        p->exit_cv.wait(lk);

    p->input_manager->stop();
    p->compositor->stop();
}

void mir::DisplayServer::stop()
{
    std::unique_lock<std::mutex> lk(p->exit_guard);
    p->exit = true;
    p->exit_cv.notify_one();
}
