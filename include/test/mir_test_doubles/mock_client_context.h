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
 * Authored by: Alexandros Frantzis <alexandros.frantzis@canonical.com>
 */

#ifndef MIR_TEST_DOUBLES_MOCK_CLIENT_CONTEXT_H_
#define MIR_TEST_DOUBLES_MOCK_CLIENT_CONTEXT_H_

#include "mir_toolkit/mir_client_library.h"
#include "src/client/client_context.h"

#include <gmock/gmock.h>

namespace mir
{
namespace test
{
namespace doubles
{
struct MockClientContext : public client::ClientContext
{
    MockClientContext()
        : connection{reinterpret_cast<MirConnection*>(0xabcdef)}
    {
        using namespace testing;

        ON_CALL(*this, mir_connection()).WillByDefault(Return(connection));
        EXPECT_CALL(*this, mir_connection()).Times(AtLeast(0));
        EXPECT_CALL(*this, populate(_)).Times(AtLeast(0));
    }

    MirConnection* connection;

    MOCK_METHOD0(mir_connection, MirConnection*());
    MOCK_METHOD1(populate, void(MirPlatformPackage&));
};

}
}
}

#endif /* MIR_TEST_DOUBLES_MOCK_CLIENT_CONTEXT_H_ */
