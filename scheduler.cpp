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

// $Revision: 7004 $ $Date:: 2017-05-16 #$ $Author: serge $

#include "scheduler.h"      // self

#include "utils/mutex_helper.h"     // MUTEX_SCOPE_LOCK
#include "utils/dummy_logger.h"     // dummy_log
#include "utils/assert.h"           // ASSERT


namespace scheduler
{
Scheduler::Scheduler(
        const Duration & granularity ):
        granularity_( granularity ),
        has_next_exec_time_( false )
{
}

void Scheduler::thread_func()
{
    utils::set_this_thread_name( "scheduler" );

//    dummy_log_info( MODULENAME, "thread_func: started" );

    bool should_run    = true;
    while( should_run )
    {
    }

//    dummy_log_info( MODULENAME, "thread_func: ended" );
}


bool Scheduler::insert_job( job_id_t * job_id, std::string * error, IJob * job )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    auto exec_time = job->get_exec_time();

    if( is_valid( exec_time ) == false )
    {
        * error = "exec_time lies in the past";
        return false;
    }

    * job_id = get_next_job_id();

    auto b = map_id_to_job_.insert( std::make_pair( * job_id, job ) );

    ASSERT( b ); (void)b;

    schedule_job_to_time( * job_id, exec_time );

    return true;
}

bool Scheduler::modify_job( std::string * error, job_id_t job_id, const Time & exec_time )
{
    MUTEX_SCOPE_LOCK( mutex_ );

    if( is_valid( exec_time ) == false )
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

bool Scheduler::is_valid( const Time & exec_time )
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

} //namespace scheduler
