/*

Scheduler - PeriodicJob Aux.

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

// $Revision: 7106 $ $Date:: 2017-07-07 #$ $Author: serge $

#ifndef SCHEDULER_PERIODIC_JOB_AUX_H
#define SCHEDULER_PERIODIC_JOB_AUX_H

#include "periodic_job.h"       // PeriodicJob
#include "time_convert.h"       // epoch_sec_to_time

namespace scheduler
{

template< class CLOSURE >
inline PeriodicJob<CLOSURE> *create_periodic_job( const std::string & descr, const Time & exec_time, const Duration & period, const CLOSURE & closure )
{
    return new PeriodicJob<CLOSURE>( descr, exec_time, period, closure );
}

template< class CLOSURE >
inline PeriodicJob<CLOSURE> *create_periodic_job( const std::string & descr, uint32_t exec_time_epoch_sec, uint32_t period_sec, const CLOSURE & closure )
{
    auto exec_time      = epoch_sec_to_time( exec_time_epoch_sec );

    auto period         = std::chrono::seconds( period_sec );

    return create_periodic_job( descr, exec_time, period, closure );
}

template< class CLOSURE >
inline bool create_and_insert_periodic_job(
        job_id_t            * job_id,
        std::string         * error_msg,
        IScheduler          & sched,
        const std::string   & descr,
        const Time          & exec_time,
        const Duration      & period,
        const CLOSURE       & closure )
{
    auto job = create_periodic_job( descr, exec_time, period, closure );

    auto res = sched.insert_job( job_id, error_msg, job );

    if( res == false )
    {
        delete job;
    }

    return res;
}

template< class CLOSURE >
inline bool create_and_insert_periodic_job(
        job_id_t            * job_id,
        std::string         * error_msg,
        IScheduler          & sched,
        const std::string   & descr,
        uint32_t            exec_time_epoch_sec,
        uint32_t            period_sec,
        const CLOSURE       & closure )
{
    auto job = create_periodic_job( descr, exec_time_epoch_sec, period_sec, closure );

    auto res = sched.insert_job( job_id, error_msg, job );

    if( res == false )
    {
        delete job;
    }

    return res;
}

} // namespace scheduler

#endif // SCHEDULER_PERIODIC_JOB_AUX_H
