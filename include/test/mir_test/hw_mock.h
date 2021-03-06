/*
 * Copyright © 2013 Canonical Ltd.
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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */
#ifndef MIR_TEST_DOUBLES_MOCK_ANDROID_HWC_DEVICE_H_
#define MIR_TEST_DOUBLES_MOCK_ANDROID_HWC_DEVICE_H_

#include "mir_test_doubles/mock_android_alloc_device.h"
#include "mir_test_doubles/mock_hwc_composer_device_1.h"

#include <hardware/hardware.h>
#include <gmock/gmock.h>
#include <memory>

namespace mir
{
namespace test
{

typedef struct hw_module_t hw_module;
class HardwareModuleStub : public hw_module
{
public:
    HardwareModuleStub(hw_device_t& device);

    static int hw_open(const struct hw_module_t* module, const char*, struct hw_device_t** device);
    static int hw_close(struct hw_device_t*);

    hw_module_methods_t gr_methods;
    hw_device_t& mock_hw_device;
};

class HardwareAccessMock
{
public:
    HardwareAccessMock();
    ~HardwareAccessMock();

    MOCK_METHOD2(hw_get_module, int(const char *id, const struct hw_module_t**));

    std::shared_ptr<alloc_device_t> mock_alloc_device;
    std::shared_ptr<hwc_composer_device_1> mock_hwc_device;

    std::shared_ptr<HardwareModuleStub> mock_gralloc_module;
    std::shared_ptr<HardwareModuleStub> mock_hwc_module;
};

}
}

#endif /* MIR_TEST_DOUBLES_MOCK_ANDROID_HWC_DEVICE_H_ */
