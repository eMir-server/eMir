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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#ifndef MIR_CLIENT_CLIENT_SURFACE_H_
#define MIR_CLIENT_CLIENT_SURFACE_H_

#include "mir_toolkit/mir_client_library.h"

#include <memory>
namespace mir
{
namespace client
{
class ClientBuffer;
class ClientSurface
{
  public:
    virtual mir_toolkit::MirSurfaceParameters get_parameters() const = 0;
    virtual std::shared_ptr<ClientBuffer> get_current_buffer() = 0;
    virtual mir_toolkit::MirWaitHandle* next_buffer(mir_toolkit::mir_surface_lifecycle_callback callback, void * context) = 0;
  protected:
    ClientSurface() {}
    virtual ~ClientSurface() {}

    ClientSurface(const ClientSurface&) = delete;
    ClientSurface& operator=(const ClientSurface&) = delete;
};

}
}

#endif /* MIR_CLIENT_CLIENT_SURFACE_H_ */
