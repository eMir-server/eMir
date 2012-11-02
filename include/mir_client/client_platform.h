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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */
#ifndef MIR_CLIENT_CLIENT_PLATFORM_H_
#define MIR_CLIENT_CLIENT_PLATFORM_H_

#include <memory>

namespace mir
{
namespace client
{
class ClientBufferDepository;

class ClientPlatform
{
public:
    ClientPlatform() = default;
    ClientPlatform(const ClientPlatform& p) = delete;
    ClientPlatform& operator=(const ClientPlatform& p) = delete;

    virtual std::shared_ptr<ClientBufferDepository> create_platform_depository () = 0;
};

std::shared_ptr<ClientPlatform> create_client_platform();

}
}

#endif /* MIR_CLIENT_CLIENT_PLATFORM_H_ */