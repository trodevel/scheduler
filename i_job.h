/*

Scheduler - IJob.

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

// $Revision: 7015 $ $Date:: 2017-06-08 #$ $Author: serge $

#ifndef SCHEDULER_I_JOB_H
#define SCHEDULER_I_JOB_H

#include "my_time.h"    // Time
#include "duration.h"   // Duration

namespace scheduler
{

class IJob
{
public:

    virtual ~IJob() {}

    virtual void invoke()                                   = 0;
    virtual bool is_periodic() const                        = 0;
    virtual const Time & get_exec_time() const              = 0;
    virtual const Duration & get_period() const             = 0;

    virtual void set_exec_time( const Time & exec_time )    = 0;
};

} // namespace scheduler

#endif // SCHEDULER_I_JOB_H
