/*

Scheduler - OneTimeJob.

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

// $Revision: 6980 $ $Date:: 2017-05-15 #$ $Author: serge $

#ifndef SCHEDULER_ONE_TIME_JOB_H
#define SCHEDULER_ONE_TIME_JOB_H

namespace scheduler
{

template< class CLOSURE >
class OneTimeJob: public Job
{
public:

    OneTimeJob(
            const std::string   & descr,
            const Time          & exec_time,
            const CLOSURE       & closure ) :
            Job( descr, exec_time ),
            closure_( closure )
    {
    }

    void invoke()
    {
        //dummy_log_trace( "OneTimeJob", "invoke()" );

        closure_();
    }

    virtual bool is_periodic() const
    {
        return false;
    }

    virtual const Duration & get_period()
    {
        static const Duration dummy;

        return dummy;
    }


public:
    CLOSURE     closure_;
};

} // namespace scheduler

#endif // SCHED_LIB_ONE_TIME_JOB_H
