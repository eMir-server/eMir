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
 * Authored by: Robert Carr <robert.carr@canonical.com>
 */

#include "event_filter_chain.h"

namespace mi = mir::input;

mi::EventFilterChain::EventFilterChain(std::initializer_list<std::shared_ptr<mi::EventFilter> const> values) :
    filters(values.begin(), values.end())
{
}

bool mi::EventFilterChain::handles(const MirEvent &event)
{
    for (auto it = filters.begin(); it != filters.end(); it++)
    {
        auto filter = *it;
        if (filter->handles(event)) return true;
    }
    return false;
}

