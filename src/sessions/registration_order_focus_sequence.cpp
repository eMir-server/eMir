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
 * Authored By: Robert Carr <robert.carr@canonical.com>
 */

#include "mir/sessions/registration_order_focus_sequence.h"
#include "mir/sessions/session.h"
#include "mir/sessions/session_container.h"

#include <memory>
#include <cassert>
#include <algorithm>


namespace msess = mir::sessions;

msess::RegistrationOrderFocusSequence::RegistrationOrderFocusSequence(std::shared_ptr<msess::SessionContainer> const& app_container) :
  session_container(app_container)
{

}

std::weak_ptr<msess::Session> msess::RegistrationOrderFocusSequence::successor_of(std::shared_ptr<msess::Session> const& focused_app) const
{
    std::shared_ptr<msess::Session> first;
    std::shared_ptr<msess::Session> result;
    bool found{false};

    session_container->for_each(
        [&](std::shared_ptr<msess::Session> const& session)
         {
             if (!first) first = session;

             if (found)
             {
                 if (!result) result = session;
             }
             else if (focused_app == session)
             {
                 found = true;
             }
         });

    if (result) return result;
    return first;
}

std::weak_ptr<msess::Session> msess::RegistrationOrderFocusSequence::predecessor_of(std::shared_ptr<msess::Session> const& focused_app) const
{
    std::shared_ptr<msess::Session> last;
    std::shared_ptr<msess::Session> result;
    bool found{false};

    session_container->for_each(
        [&](std::shared_ptr<msess::Session> const& session)
        {
            last = session;

            if (focused_app == session)
            {
                found = true;
            }
            else if (!found)
            {
                result = session;
            }
        });

    if (result) return result;
    return last;
}