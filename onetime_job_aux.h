/*

Scheduler - OneTimeJob Aux.

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

// $Revision: 7027 $ $Date:: 2017-06-09 #$ $Author: serge $

#ifndef SCHEDULER_ONE_TIME_JOB_AUX_H
#define SCHEDULER_ONE_TIME_JOB_AUX_H

#include "onetime_job.h"                // OneTimeJob

namespace scheduler
{

template< class CLOSURE >
inline OneTimeJob<CLOSURE> *create_one_time_job( const std::string & descr, const Time & exec_time, const CLOSURE & closure )
{
    return new OneTimeJob<CLOSURE>( descr, exec_time, closure );
}

template< class CLOSURE >
inline OneTimeJob<CLOSURE> *create_one_time_job( const std::string & descr, uint32_t exec_time_epoch_sec, const CLOSURE & closure )
{
    auto epoch          = Time();
    auto since_epoch    = std::chrono::seconds( exec_time_epoch_sec );
    auto exec_time      = epoch + since_epoch;

    return new OneTimeJob<CLOSURE>( descr, exec_time, closure );
}

} // namespace scheduler

#endif // SCHEDULER_ONE_TIME_JOB_AUX_H
