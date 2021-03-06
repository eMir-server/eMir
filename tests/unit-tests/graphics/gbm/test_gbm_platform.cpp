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

#include "mir/graphics/platform.h"
#include "mir/graphics/platform_ipc_package.h"
#include "mir/graphics/drm_authenticator.h"

#include "mir/graphics/null_display_report.h"

#include <gtest/gtest.h>

#include "mock_drm.h"
#include "mock_gbm.h"

#include <gtest/gtest.h>

#include <stdexcept>

namespace mg = mir::graphics;

namespace
{
class GBMGraphicsPlatform : public ::testing::Test
{
public:
    void SetUp()
    {
        ::testing::Mock::VerifyAndClearExpectations(&mock_drm);
        ::testing::Mock::VerifyAndClearExpectations(&mock_gbm);
    }
    ::testing::NiceMock<mg::gbm::MockDRM> mock_drm;
    ::testing::NiceMock<mg::gbm::MockGBM> mock_gbm;
};
}

TEST_F(GBMGraphicsPlatform, get_ipc_package)
{
    using namespace testing;
    const int auth_fd{66};

    /* First time for master DRM fd, second for authenticated fd */
    EXPECT_CALL(mock_drm, drmOpen(_,_))
        .Times(2)
        .WillOnce(Return(mock_drm.fake_drm.fd()))
        .WillOnce(Return(auth_fd));

    /* Expect proper authorization */
    EXPECT_CALL(mock_drm, drmGetMagic(auth_fd,_));
    EXPECT_CALL(mock_drm, drmAuthMagic(mock_drm.fake_drm.fd(),_));

    EXPECT_CALL(mock_drm, drmClose(mock_drm.fake_drm.fd()));

    /* Expect authenticated fd to be closed when package is destroyed */
    EXPECT_CALL(mock_drm, drmClose(auth_fd));

    EXPECT_NO_THROW (
        auto platform = mg::create_platform(std::make_shared<mg::NullDisplayReport>());
        auto pkg = platform->get_ipc_package();

        ASSERT_TRUE(pkg.get());
        ASSERT_EQ(std::vector<int32_t>::size_type{1}, pkg->ipc_fds.size());
        ASSERT_EQ(auth_fd, pkg->ipc_fds[0]);
    );
}

TEST_F(GBMGraphicsPlatform, a_failure_while_creating_a_platform_results_in_an_error)
{
    using namespace ::testing;

    EXPECT_CALL(mock_drm, drmOpen(_,_))
            .WillRepeatedly(Return(-1));

    try
    {
        auto platform = mg::create_platform(std::make_shared<mg::NullDisplayReport>());
    } catch(...)
    {
        return;
    }

    FAIL() << "Expected an exception to be thrown.";
}

TEST_F(GBMGraphicsPlatform, drm_auth_magic_calls_drm_function_correctly)
{
    using namespace testing;

    drm_magic_t const magic{0x10111213};

    EXPECT_CALL(mock_drm, drmAuthMagic(mock_drm.fake_drm.fd(),magic))
        .WillOnce(Return(0));

    auto platform = mg::create_platform(std::make_shared<mg::NullDisplayReport>());
    auto authenticator = std::dynamic_pointer_cast<mg::DRMAuthenticator>(platform);
    authenticator->drm_auth_magic(magic);
}

TEST_F(GBMGraphicsPlatform, drm_auth_magic_throws_if_drm_function_fails)
{
    using namespace testing;

    drm_magic_t const magic{0x10111213};

    EXPECT_CALL(mock_drm, drmAuthMagic(mock_drm.fake_drm.fd(),magic))
        .WillOnce(Return(-1));

    auto platform = mg::create_platform(std::make_shared<mg::NullDisplayReport>());
    auto authenticator = std::dynamic_pointer_cast<mg::DRMAuthenticator>(platform);

    EXPECT_THROW({
        authenticator->drm_auth_magic(magic);
    }, std::runtime_error);
}
