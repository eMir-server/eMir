/*
 * Copyright © 2013 Canonical Ltd.
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

#include <boost/throw_exception.hpp>
#include "mir/graphics/display_configuration.h"
#include "mir/graphics/display.h"
#include "src/graphics/gbm/gbm_platform.h"

#include "mir_test/egl_mock.h"
#include "mir_test/gl_mock.h"
#include "mir_test_doubles/null_display_listener.h"

#include "mock_drm.h"
#include "mock_gbm.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mg = mir::graphics;
namespace mgg = mir::graphics::gbm;
namespace mtd = mir::test::doubles;
namespace geom = mir::geometry;

namespace
{

mg::DisplayConfigurationMode conf_mode_from_drm_mode(drmModeModeInfo const& mode)
{
    geom::Size const size{geom::Width{mode.hdisplay}, geom::Height{mode.vdisplay}};
    double vrefresh_hz{0.0};

    /* Calculate vertical refresh rate from DRM mode information */
    if (mode.htotal != 0.0 && mode.vtotal != 0.0)
    {
        vrefresh_hz = mode.clock * 1000.0 / (mode.htotal * mode.vtotal);
        vrefresh_hz = round(vrefresh_hz * 10.0) / 10.0;
    }

    return mg::DisplayConfigurationMode{size, vrefresh_hz};
}

class GBMDisplayConfigurationTest : public ::testing::Test
{
public:
    GBMDisplayConfigurationTest() :
        null_listener{std::make_shared<mtd::NullDisplayListener>()}
    {
        using namespace testing;

        /* Needed for display start-up */
        ON_CALL(mock_egl, eglChooseConfig(_,_,_,1,_))
        .WillByDefault(DoAll(SetArgPointee<2>(mock_egl.fake_configs[0]),
                             SetArgPointee<4>(1),
                             Return(EGL_TRUE)));

        const char* egl_exts = "EGL_KHR_image EGL_KHR_image_base EGL_MESA_drm_image";
        const char* gl_exts = "GL_OES_texture_npot GL_OES_EGL_image";

        ON_CALL(mock_egl, eglQueryString(_,EGL_EXTENSIONS))
        .WillByDefault(Return(egl_exts));
        ON_CALL(mock_gl, glGetString(GL_EXTENSIONS))
        .WillByDefault(Return(reinterpret_cast<const GLubyte*>(gl_exts)));

        setup_sample_modes();
    }

    void setup_sample_modes()
    {
        /* Add DRM modes */
        modes0.push_back(mgg::FakeDRMResources::create_mode(1920, 1080, 138500, 2080, 1111));
        modes0.push_back(mgg::FakeDRMResources::create_mode(1920, 1080, 148500, 2200, 1125));
        modes0.push_back(mgg::FakeDRMResources::create_mode(1680, 1050, 119000, 1840, 1080));
        modes0.push_back(mgg::FakeDRMResources::create_mode(832, 624, 57284, 1152, 667));

        /* Add the DisplayConfiguration modes corresponding to the DRM modes */
        for (auto const& mode : modes0)
            conf_modes0.push_back(conf_mode_from_drm_mode(mode));
    }

    ::testing::NiceMock<mir::EglMock> mock_egl;
    ::testing::NiceMock<mir::GLMock> mock_gl;
    ::testing::NiceMock<mgg::MockDRM> mock_drm;
    ::testing::NiceMock<mgg::MockGBM> mock_gbm;
    std::shared_ptr<mg::DisplayListener> const null_listener;

    std::vector<drmModeModeInfo> modes0;
    std::vector<mg::DisplayConfigurationMode> conf_modes0;
    std::vector<drmModeModeInfo> modes_empty;
};

}

TEST_F(GBMDisplayConfigurationTest, configuration_is_read_correctly)
{
    using namespace ::testing;

    /* Set up DRM resources */
    uint32_t const invalid_id{0};
    uint32_t const crtc0_id{10};
    uint32_t const encoder0_id{20};
    uint32_t const encoder1_id{21};
    uint32_t const connector0_id{30};
    uint32_t const connector1_id{31};
    uint32_t const connector2_id{32};
    geom::Size const connector0_physical_size_mm{geom::Width{480}, geom::Height{270}};
    geom::Size const connector1_physical_size_mm{};
    geom::Size const connector2_physical_size_mm{};

    mgg::FakeDRMResources& resources(mock_drm.fake_drm);

    resources.reset();

    resources.add_crtc(crtc0_id, modes0[1]);

    resources.add_encoder(encoder0_id, crtc0_id);
    resources.add_encoder(encoder1_id, invalid_id);

    resources.add_connector(connector0_id, DRM_MODE_CONNECTED, encoder0_id,
                            modes0, connector0_physical_size_mm);
    resources.add_connector(connector1_id, DRM_MODE_DISCONNECTED, invalid_id,
                            modes_empty, connector1_physical_size_mm);
    resources.add_connector(connector2_id, DRM_MODE_DISCONNECTED, encoder1_id,
                            modes_empty, connector2_physical_size_mm);

    resources.prepare();

    /* Expected results */
    std::vector<mg::DisplayConfigurationOutput> const expected_outputs =
    {
        {
            mg::DisplayConfigurationOutputId{0},
            mg::DisplayConfigurationCardId{0},
            conf_modes0,
            connector0_physical_size_mm,
            true,
            1
        },
        {
            mg::DisplayConfigurationOutputId{1},
            mg::DisplayConfigurationCardId{0},
            std::vector<mg::DisplayConfigurationMode>(),
            connector1_physical_size_mm,
            false,
            std::numeric_limits<size_t>::max()
        },
        {
            mg::DisplayConfigurationOutputId{2},
            mg::DisplayConfigurationCardId{0},
            std::vector<mg::DisplayConfigurationMode>(),
            connector2_physical_size_mm,
            false,
            std::numeric_limits<size_t>::max()
        }
    };

    /* Test body */
    auto platform = std::make_shared<mgg::GBMPlatform>(null_listener);
    auto display = platform->create_display();

    auto conf = display->configuration();

    size_t output_count{0};

    conf->for_each_output([&](mg::DisplayConfigurationOutput const& output)
    {
        ASSERT_LT(output_count, expected_outputs.size());
        EXPECT_EQ(expected_outputs[output_count], output) << "output_count: " << output_count;
        ++output_count;
    });

    EXPECT_EQ(expected_outputs.size(), output_count);
}