/*

Scheduler - OneTimeJob (Timeout) Aux.

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

#ifndef SCHEDULER_TIMEOUT_JOB_AUX_H
#define SCHEDULER_TIMEOUT_JOB_AUX_H

#include "onetime_job_aux.h"        // create_one_time_job
#include "time_convert.h"           // get_now()

namespace scheduler
{

template< class CLOSURE >
inline OneTimeJob<CLOSURE> *create_timeout_job( const std::string & descr, const Duration & timeout, const CLOSURE & closure )
{
    auto now        = get_now();
    Time exec_time  = now + timeout;

    return create_one_time_job( descr, exec_time, closure );
}

template< class CLOSURE >
inline OneTimeJob<CLOSURE> *create_timeout_job( const std::string & descr, uint32_t timeout_sec, const CLOSURE & closure )
{
    Duration timeout    = std::chrono::seconds( timeout_sec );

    return create_timeout_job( descr, timeout, closure );
}

template< class CLOSURE >
inline bool create_and_insert_timeout_job(
        job_id_t            * job_id,
        std::string         * error_msg,
        IScheduler          & sched,
        const std::string   & descr,
        const Duration      & timeout,
        const CLOSURE       & closure )
{
    auto job = create_timeout_job( descr, timeout, closure );

    auto res = sched.insert_job( job_id, error_msg, job );

    if( res == false )
    {
        delete job;
    }

    return res;
}

template< class CLOSURE >
inline bool create_and_insert_timeout_job(
        job_id_t            * job_id,
        std::string         * error_msg,
        IScheduler          & sched,
        const std::string   & descr,
        uint32_t            timeout_sec,
        const CLOSURE       & closure )
{
    auto job = create_timeout_job( descr, timeout_sec, closure );

    auto res = sched.insert_job( job_id, error_msg, job );

    if( res == false )
    {
        delete job;
    }

    return res;
}

} // namespace scheduler

#endif // SCHEDULER_TIMEOUT_JOB_AUX_H
