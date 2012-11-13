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
 *              Daniel d'Andrada <daniel.dandrada@canonical.com>
 */
#include "mir/input/event_filter.h"
#include "src/input/android/android_input_manager.h"
#include "mir/thread/all.h"

#include "mir_test/empty_deleter.h"
#include "mir_test/fake_event_hub.h"
#include "mir_test/mock_event_filter.h"
#include "mir_test/wait_condition.h"
#include "mir_test/event_factory.h"

// Needed implicitly for InputManager destructor because of android::sp :/
#include <InputDispatcher.h>
#include <InputReader.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mi = mir::input;
namespace mia = mir::input::android;
namespace mis = mir::input::synthesis;

using mir::MockEventFilter;
using mir::WaitCondition;

namespace
{

class AndroidInputManagerAndEventFilterDispatcherSetup : public testing::Test
{
  public:
    void SetUp()
    {
        event_hub = new mia::FakeEventHub();
        input_manager.reset(new mia::InputManager(
            event_hub,
            {std::shared_ptr<mi::EventFilter>(&event_filter, mir::EmptyDeleter())}));
        input_manager->start();
    }

    void TearDown()
    {
        input_manager->stop();
    }

  protected:
    android::sp<mia::FakeEventHub> event_hub;
    MockEventFilter event_filter;
    std::shared_ptr<mia::InputManager> input_manager;
};
}

ACTION_P(ReturnFalseAndWakeUp, wait_condition)
{
    wait_condition->wake_up_everyone();
    return false;
}

TEST_F(AndroidInputManagerAndEventFilterDispatcherSetup, manager_dispatches_key_events_to_filter)
{
    using namespace ::testing;

    WaitCondition wait_condition;

    EXPECT_CALL(
        event_filter,
        handles(KeyDownEvent()))
            .Times(1)
            .WillOnce(ReturnFalseAndWakeUp(&wait_condition));

    event_hub->synthesize_builtin_keyboard_added();
    event_hub->synthesize_device_scan_complete();

    event_hub->synthesize_event(mis::a_key_down_event()
				.of_scancode(KEY_ENTER));

    wait_condition.wait_for_seconds(30);
}

TEST_F(AndroidInputManagerAndEventFilterDispatcherSetup, manager_dispatches_button_events_to_filter)
{
    using namespace ::testing;

    WaitCondition wait_condition;

    EXPECT_CALL(
        event_filter,
        handles(ButtonDownEvent()))
            .Times(1)
            .WillOnce(ReturnFalseAndWakeUp(&wait_condition));

    event_hub->synthesize_builtin_cursor_added();
    event_hub->synthesize_device_scan_complete();

    event_hub->synthesize_event(mis::a_button_down_event().of_button(BTN_LEFT));

    wait_condition.wait_for_seconds(30);
}

TEST_F(AndroidInputManagerAndEventFilterDispatcherSetup, manager_dispatches_motion_events_to_filter)
{
    using namespace ::testing;

    WaitCondition wait_condition;

    // We get absolute motion events since we have a pointer controller.
    {
	InSequence seq;

	EXPECT_CALL(event_filter,
		    handles(MotionEvent(100, 100)))
	    .WillOnce(Return(false));
	EXPECT_CALL(event_filter,
		    handles(MotionEvent(200, 100)))
	    .WillOnce(ReturnFalseAndWakeUp(&wait_condition));
    }

    event_hub->synthesize_builtin_cursor_added();
    event_hub->synthesize_device_scan_complete();

    event_hub->synthesize_event(mis::a_motion_event().with_movement(100,100));
    event_hub->synthesize_event(mis::a_motion_event().with_movement(100,0));
    
    wait_condition.wait_for_seconds(30);
}
