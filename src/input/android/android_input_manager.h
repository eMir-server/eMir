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
 *              Daniel d'Andradra <daniel.dandrada@canonical.com>
 */

#ifndef MIR_INPUT_ANDROID_INPUT_MANAGER_H_
#define MIR_INPUT_ANDROID_INPUT_MANAGER_H_

#include "mir/input/input_manager.h"
#include "../event_filter_chain.h"

#include <EventHub.h>
#include <initializer_list>

namespace android
{
class InputDispatcher;
class InputDispatcherThread;
class InputReader;
class InputReaderThread;
}

namespace droidinput = android;

namespace mir
{
namespace input
{
namespace android
{

class InputManager : public mir::input::InputManager
{
public:
    explicit InputManager(droidinput::sp<droidinput::EventHubInterface> event_hub,
			  std::initializer_list<std::shared_ptr<input::EventFilter> const> filters);
    virtual ~InputManager() {}

    virtual void start();
    virtual void stop();
protected:
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

private:
    droidinput::sp<droidinput::EventHubInterface> event_hub;
    droidinput::sp<droidinput::InputDispatcher> dispatcher;
    droidinput::sp<droidinput::InputReader> reader;

    // It's important to keep droidinput::sp to dispatcher_thread
    // and reader_thread or they will free themselves on exit.
    droidinput::sp<droidinput::InputDispatcherThread> dispatcher_thread;
    droidinput::sp<droidinput::InputReaderThread> reader_thread;
    
    std::shared_ptr<EventFilterChain> filter_chain;
};

}
}
}

#endif // MIR_INPUT_INPUT_MANAGER