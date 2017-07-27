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

// $Revision: 7155 $ $Date:: 2017-07-11 #$ $Author: serge $

#ifndef SCHEDULER_TIME_CONVERT_H
#define SCHEDULER_TIME_CONVERT_H

#include "duration.h"   // Duration
#include "my_time.h"    // Time

namespace scheduler
{

inline Time get_now()
{
    auto now = std::chrono::system_clock::now();

    return now;
}

inline Time epoch_sec_to_time( uint32_t since_epoch_sec )
{
    auto epoch          = Time();
    auto since_epoch    = std::chrono::seconds( since_epoch_sec );
    auto res            = epoch + since_epoch;

    return res;
}

} //namespace scheduler

#endif // SCHEDULER_TIME_CONVERT_H
