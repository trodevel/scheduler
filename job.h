/*

Scheduler - Job id.

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

// $Revision: 7062 $ $Date:: 2017-06-13 #$ $Author: serge $

#ifndef SCHEDULER_JOB_H
#define SCHEDULER_JOB_H

#include "i_job.h"  // IJob

namespace scheduler
{

class Job: public IJob
{
protected:

    Job( const std::string & descr, const Time & exec_time ):
        descr_( descr ),
        exec_time_( exec_time )
    {
    }

public:

    virtual const Time & get_exec_time() const
    {
        return exec_time_;
    }

    virtual void set_exec_time( const Time & exec_time )
    {
        exec_time_  = exec_time;
    }

protected:

    std::string     descr_;

    Time            exec_time_;
};

} // namespace scheduler

#endif // SCHEDULER_JOB_H
