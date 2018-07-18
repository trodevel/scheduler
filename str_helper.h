/*

Scheduler - Time.

Copyright (C) 2017 Sergey Kolevatov

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// $Revision: 9547 $ $Date:: 2018-07-18 #$ $Author: serge $

#ifndef SCHEDULER__STR_HELPER_H
#define SCHEDULER__STR_HELPER_H

#include <string>

#include "time_convert.h"    // to_epoch_microsec

namespace scheduler
{

inline std::string to_string( const Time & tp )
{
    auto epoch = to_epoch_microsec( tp );

    auto sec    = epoch / 1000000;
    auto ms     = epoch % 1000000;

    std::string res  = std::to_string( sec ) + "." + std::to_string( ms );

    return res;
}

} //namespace scheduler

#endif // SCHEDULER__STR_HELPER_H
