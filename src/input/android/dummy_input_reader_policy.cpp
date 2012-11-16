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

#include "dummy_input_reader_policy.h"

namespace mia = mir::input::android;

void mia::DummyInputReaderPolicy::getReaderConfiguration(droidinput::InputReaderConfiguration* out_config)
{
    static const int32_t default_display_id = 0;
    static const bool is_external = false;
    static const int32_t display_width = 1024;
    static const int32_t display_height = 1024;
    static const int32_t display_orientation = droidinput::DISPLAY_ORIENTATION_0;
    // TODO: This needs to go.
    out_config->setDisplayInfo(
        default_display_id,
        is_external,
        display_width,
        display_height,
        display_orientation);
}

droidinput::sp<droidinput::PointerControllerInterface> mia::DummyInputReaderPolicy::obtainPointerController(int32_t device_id)
{
    (void)device_id;
    return pointer_controller;
}

void mia::DummyInputReaderPolicy::notifyInputDevicesChanged(
    const droidinput::Vector<droidinput::InputDeviceInfo>& input_devices)
{
    (void)input_devices;
}

droidinput::sp<droidinput::KeyCharacterMap> mia::DummyInputReaderPolicy::getKeyboardLayoutOverlay(
    const droidinput::String8& input_device_descriptor)
{
    (void)input_device_descriptor;
    return droidinput::KeyCharacterMap::empty();
}

droidinput::String8 mia::DummyInputReaderPolicy::getDeviceAlias(const droidinput::InputDeviceIdentifier& identifier)
{
    (void)identifier;
    return droidinput::String8();
}
