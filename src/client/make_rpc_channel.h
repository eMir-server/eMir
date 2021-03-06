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
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */
#ifndef MIR_CLIENT_MAKE_RPC_CHANNEL_H_
#define MIR_CLIENT_MAKE_RPC_CHANNEL_H_

#include <memory>
#include "mir_protobuf.pb.h"

namespace mir
{
namespace client
{
class Logger;

std::shared_ptr<google::protobuf::RpcChannel>
make_rpc_channel(std::string const& name, std::shared_ptr<Logger> const& log);
}
}

#endif /* MIR_CLIENT_MAKE_RPC_CHANNEL_H_ */
