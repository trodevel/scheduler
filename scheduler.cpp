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

// $Revision: 7441 $ $Date:: 2017-07-27 #$ $Author: serge $

#include "scheduler.h"      // self

#include <functional>               // std::bind
#include <algorithm>                // std::find

#include "utils/mutex_helper.h"     // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"     // dummy_log
#include "utils/assert.h"           // ASSERT
#include "utils/set_this_thread_name.h"     // set_this_thread_name()

namespace scheduler
{
Scheduler::Scheduler(
        const Duration & granularity ):
        granularity_( granularity ),
        should_run_( false ),
        has_next_exec_time_( false )
{
}

void Scheduler::run()
{
    if( should_run_ == false )
    {
        should_run_ = true;

        thread_ = std::thread( std::bind( &Scheduler::thread_func, this ) );
    }
}

void Scheduler::shutdown()
{
    if( should_run_ )
    {
        should_run_ = false;

        thread_.join();
    }
}

void Scheduler::thread_func()
{
    utils::set_this_thread_name( "scheduler" );

//    dummy_log_info( MODULENAME, "thread_func: started" );

    while( should_run_ )
    {
        current_time_ = get_now();

        iterate( current_time_ );

        THIS_THREAD_SLEEP( granularity_ );
    }

//    dummy_log_info( MODULENAME, "thread_func: ended" );
}

void Scheduler::iterate( const Time & curr_time )
{
    VectJob jobs;

    iterate_and_collect( & jobs, curr_time );

    invoke_jobs( jobs );
}

void Scheduler::iterate_and_collect( VectJob * jobs, const Time & curr_time )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( has_next_exec_time_ == false || curr_time < next_exec_time_ )
        return;

    if( curr_time >= next_exec_time_ )
    {
        if( events_.empty() )
        {
            throw std::logic_error( "event list is empty" );
        }

        auto it = events_.begin();

        if( it->first !=  next_exec_time_ )
        {
            throw std::logic_error( "time of first event doesn't match next_exec_time_" );
        }

        auto & job_ids = it->second;

        collect_and_postprocess_jobs( jobs, next_exec_time_, job_ids );

        events_.erase( it );

        if( events_.empty() )
        {
            has_next_exec_time_  = false;
        }
        else
        {
            next_exec_time_ = events_.begin()->first;
        }
    }
}

void Scheduler::collect_and_postprocess_jobs( VectJob * jobs, const Time & exec_time, const VectJobId & job_ids )
{
    for( auto & job_id : job_ids )
    {
        collect_and_postprocess_job( jobs, exec_time, job_id );
    }
}

void Scheduler::collect_and_postprocess_job( VectJob * jobs, const Time & exec_time, job_id_t job_id )
{
    auto job = find_job( job_id );

    if( job == nullptr )
        return;

    jobs->push_back( job );

    post_invoke( exec_time, job_id, * job );
}

void Scheduler::post_invoke( const Time & exec_time, job_id_t job_id, IJob & job )
{
    if( job.is_periodic() )
    {
        auto new_exec_time = exec_time + job.get_period();

        job.set_exec_time( new_exec_time );

        schedule_job_to_time( job_id, new_exec_time );
    }
    else
    {
        delete_job( job_id );
    }
}

void Scheduler::invoke_jobs( VectJob & jobs )
{
    for( auto & e : jobs )
    {
        e->invoke();
    }
}

bool Scheduler::insert_job( job_id_t * job_id, std::string * error, IJob * job )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( is_valid( error, * job ) == false )
    {
        return false;
    }

    * job_id = get_next_job_id();

    auto b = map_id_to_job_.insert( std::make_pair( * job_id, job ) ).second;

    ASSERT( b ); (void)b;

    auto exec_time = job->get_exec_time();

    schedule_job_to_time( * job_id, exec_time );

    return true;
}

bool Scheduler::modify_job( std::string * error, job_id_t job_id, const Time & exec_time )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( is_time_valid( exec_time ) == false )
    {
        * error = "new exec_time lies in the past";
        return false;
    }

    auto job = find_job( job_id );

    if( job == nullptr )
    {
        * error = "cannot find job_id " + std::to_string( job_id );
        return false;
    }

    auto old_exec_time = job->get_exec_time();

    job->set_exec_time( exec_time );

    reschedule_job_to_time( job_id, old_exec_time, exec_time );

    return true;
}

bool Scheduler::delete_job( std::string * error, job_id_t job_id )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    auto job = find_job( job_id );

    if( job == nullptr )
    {
        * error = "cannot find job_id " + std::to_string( job_id );
        return false;
    }

    auto exec_time = job->get_exec_time();

    delete_job_at_time( job_id, exec_time );

    return true;
}

IJob * Scheduler::find_job( job_id_t job_id )
{
    auto it = map_id_to_job_.find( job_id );

    if( it == map_id_to_job_.end() )
    {
        return nullptr;
    }

    return it->second;
}

void Scheduler::delete_job( job_id_t job_id )
{
    auto it = map_id_to_job_.find( job_id );

    if( it != map_id_to_job_.end() )
    {
        map_id_to_job_.erase( it );
    }
}

bool Scheduler::is_valid( std::string * error, const IJob & job )
{
    auto & exec_time = job.get_exec_time();

    if( is_time_valid( exec_time ) == false )
    {
        * error = "exec_time lies in the past";
        return false;
    }

    if( job.is_periodic() && job.get_period() < granularity_ )
    {
        * error = "period is less than scheduler's granularity";
        return false;
    }

    return true;
}

bool Scheduler::is_time_valid( const Time & exec_time )
{
    if( exec_time < ( current_time_ + granularity_ ) )
    {
        return false;
    }

    return true;
}

void Scheduler::delete_job_at_time( job_id_t job_id, const Time & exec_time )
{
    auto it = events_.find( exec_time );

    ASSERT( it != events_.end() );

    auto & l = it->second;

    auto it_2   = std::find( l.begin(), l.end(), job_id );

    ASSERT( it_2 != l.end() );

    l.erase( it_2 );

    if( l.empty() )
    {
        // remove time point as not events are scheduled to it
        events_.erase( it );
    }

    correct_next_exec_time_after_deletion( exec_time );
}

void Scheduler::schedule_job_to_time( job_id_t job_id, const Time & exec_time )
{
    auto it = events_.find( exec_time );

    if( it == events_.end() )
    {
        VectJobId l;

        l.push_back( job_id );

        events_.insert( std::make_pair( exec_time, l ) );

        correct_next_exec_time_after_insertion( exec_time );
    }
    else
    {
        auto & l = it->second;

        l.push_back( job_id );
    }

}

void Scheduler::reschedule_job_to_time( job_id_t job_id, const Time & old_exec_time, const Time & new_exec_time )
{
    delete_job_at_time( job_id, old_exec_time );

    schedule_job_to_time( job_id, new_exec_time );
}

void Scheduler::correct_next_exec_time_after_insertion( const Time & exec_time )
{
    if( has_next_exec_time_ )
    {
        if( exec_time < next_exec_time_ )
        {
            next_exec_time_      = exec_time;
        }
    }
    else
    {
        has_next_exec_time_  = true;
        next_exec_time_      = exec_time;
    }
}

void Scheduler::correct_next_exec_time_after_deletion( const Time & exec_time )
{
    if( next_exec_time_ == exec_time )
    {
        if( events_.empty() )
        {
            has_next_exec_time_  = false;
        }
        else
        {
            next_exec_time_  = events_.begin()->first;
        }
    }
}

job_id_t Scheduler::get_next_job_id()
{
    static job_id_t job_id = 0;

    return ++job_id;
}

Time Scheduler::get_now()
{
    auto now = std::chrono::system_clock::now();

    return now;
}

} //namespace scheduler
