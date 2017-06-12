/*

Scheduler.

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

// $Revision: 7052 $ $Date:: 2017-06-12 #$ $Author: serge $

#ifndef SCHEDULER_SCHEDULER_H
#define SCHEDULER_SCHEDULER_H

#include "i_scheduler.h"    // IScheduler

#include <atomic>           // std::atomic
#include <thread>           // std::thread
#include <mutex>            // std::mutex
#include <map>              // std::map
#include <vector>           // std::vector

namespace scheduler
{

class Scheduler: public IScheduler
{
public:
    Scheduler(
            const Duration & granularity );

    virtual ~Scheduler() {};

    void run();
    void shutdown();

    bool insert_job( job_id_t * job_id, std::string * error, IJob * job ) override;
    bool modify_job( std::string * error, job_id_t job_id, const Time & exec_time ) override;
    bool delete_job( std::string * error, job_id_t job_id ) override;

private:

    typedef std::map<job_id_t, IJob*>           MapIdToJob;
    typedef std::vector<job_id_t>               VectJobId;
    typedef std::map<Time, VectJobId>           MapTimeToVectJobId;

private:

    void thread_func();

    void iterate( const Time & curr_time );
    void execute_job_ids( const Time & exec_time, const VectJobId & job_ids );
    void execute_job_id( const Time & exec_time, job_id_t job_id );

    static job_id_t get_next_job_id();

    IJob * find_job( job_id_t job_id );
    void delete_job( job_id_t job_id );
    bool is_valid( std::string * error, const IJob & job );
    bool is_time_valid( const Time & exec_time );
    void delete_job_at_time( job_id_t job_id, const Time & exec_time );
    void schedule_job_to_time( job_id_t job_id, const Time & exec_time );
    void reschedule_job_to_time( job_id_t job_id, const Time & old_exec_time, const Time & new_exec_time );
    void correct_next_exec_time_after_insertion( const Time & exec_time );
    void correct_next_exec_time_after_deletion( const Time & exec_time );

    static Time get_now();

private:

    mutable std::mutex      mutex_;

    Duration                granularity_;

    std::atomic<bool>       should_run_;
    std::thread             thread_;

    bool                    has_next_exec_time_;
    Time                    next_exec_time_;
    Time                    current_time_;
    MapIdToJob              map_id_to_job_;
    MapTimeToVectJobId      events_;
};

} //namespace scheduler

#endif // SCHEDULER_SCHEDULER_H
