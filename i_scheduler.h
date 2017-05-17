/*

Scheduler Interface.

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

// $Revision: 6972 $ $Date:: 2017-05-15 #$ $Author: serge $

#ifndef SCHEDULER_I_SCHEDULER_H
#define SCHEDULER_I_SCHEDULER_H

#include "i_job.h"      // IJob
#include "time.h"       // Time

namespace scheduler
{

class IScheduler
{
public:
    virtual ~IScheduler() {};

    virtual bool insert_job( job_id_t * job_id, std::string * error, IJob * job, const Time & exec_time )   = 0;
    virtual bool modify_job( std::string * error, job_id_t job_id, const Time & exec_time )                 = 0;
    virtual bool delete_job( std::string * error, job_id_t job_id )                                         = 0;
};

} //namespace scheduler

#endif // SCHEDULER_I_SCHEDULER_H
