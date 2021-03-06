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
 * Authored by: Thomas Guest <thomas.guest@canonical.com>
 *              Daniel van Vugt <daniel.van.vugt@canonical.com>
 */

#ifndef MIR_CLIENT_MIR_WAIT_HANDLE_H_
#define MIR_CLIENT_MIR_WAIT_HANDLE_H_

#include <condition_variable>
#include <mutex>

namespace mir_toolkit
{
struct MirWaitHandle
{
public:
    MirWaitHandle();
    ~MirWaitHandle();

    void expect_result();
    void result_received();
    void wait_for_result();

private:
    std::mutex guard;
    std::condition_variable wait_condition;

    int expecting;
    int received;
};
}

#endif /* MIR_CLIENT_MIR_WAIT_HANDLE_H_ */
