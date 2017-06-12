#include <cstdio>

#include <sstream>          // std::ostringstream
#include <iostream>         // std::cout
#include <functional>       // std::bind

#include "utils/mutex_helper.h" // MUTEX_SCOPE_LOCK

#include "scheduler.h"          // Scheduler
#include "onetime_job_aux.h"    // create_and_insert_one_time_job
#include "periodic_job_aux.h"   // create_and_insert_periodic_job

void task( unsigned a )
{
    std::ostringstream os;

    os << "task_" << a << "\n";

    std::cout << os.str();
}

void task_s( const std::string & descr )
{
    std::ostringstream os;

    os << "task: " << descr << "\n";

    std::cout << os.str();
}

void task_inc( const std::string & descr, uint32_t & val )
{
    std::ostringstream os;

    os << "task: " << descr << ", val " << ++val << "\n";

    std::cout << os.str();
}

void test_01( scheduler::IScheduler & sched )
{
    std::string name( "test_01" );

    std::cout << name << ": sleep 5 sec, fire in 3 sec" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_s, "fire" ) );

    THIS_THREAD_SLEEP_SEC( 5 );

    std::cout << name << ": done" << std::endl;
}

void test_02( scheduler::IScheduler & sched )
{
    std::string name( "test_02" );

    std::cout << name << ": sleep 5 sec, 2 tasks fire in 3 sec" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_s, "fire1" ) );
    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_s, "fire2" ) );

    THIS_THREAD_SLEEP_SEC( 5 );

    std::cout << name << ": done" << std::endl;
}

void test_03( scheduler::IScheduler & sched )
{
    std::string name( "test_03" );

    std::cout << name << ": sleep 6 sec, fire in 3 sec, rescheduler fire to 5 sec" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time_1 = now + std::chrono::seconds( 3 );
    scheduler::Time exec_time_2 = now + std::chrono::seconds( 5 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time_1, std::bind( & task_s, "fire" ) );

    sched.modify_job( & error_msg, job_id, exec_time_2 );

    THIS_THREAD_SLEEP_SEC( 6 );

    std::cout << name << ": done" << std::endl;
}

void test_04( scheduler::IScheduler & sched )
{
    std::string name( "test_04" );

    std::cout << name << ": sleep 5 sec, scheduler fire in 3 sec, delete task" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    uint32_t val = 0;

    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_inc, "fire", val ) );

    sched.delete_job( & error_msg, job_id );

    THIS_THREAD_SLEEP_SEC( 5 );

    if( val == 1 )
    {
        std::cout << "ERROR: cancelled jobs fired: val = " << val << std::endl;
    }
    else
    {
        std::cout << "OK: cancelled job didn't fire" << std::endl;
    }

    std::cout << name << ": done" << std::endl;
}

void test_05( scheduler::IScheduler & sched )
{
    std::string name( "test_05" );

    std::cout << name << ": sleep 5 sec, set fire time in the past" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now - std::chrono::seconds( 1 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    auto res = scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_s, "fire" ) );

    if( res )
    {
        std::cout << "ERROR: job was scheduled in the past" << std::endl;
    }
    else
    {
        std::cout << "OK: job cannot be scheduled in the past: " << error_msg << std::endl;
    }

    std::cout << name << ": done" << std::endl;
}

void test_06( scheduler::IScheduler & sched )
{
    std::string name( "test_06" );

    std::cout << name << ": delete non-existing task" << std::endl;

    scheduler::job_id_t job_id  = 777;
    std::string error_msg;

    auto res = sched.delete_job( & error_msg, job_id );

    if( res )
    {
        std::cout << "ERROR: non-existing job was cancelled" << std::endl;
    }
    else
    {
        std::cout << "OK: non-existing job cannot be cancelled: " << error_msg << std::endl;
    }

    std::cout << name << ": done" << std::endl;
}

void test_07( scheduler::IScheduler & sched )
{
    std::string name( "test_07" );

    std::cout << name << ": sleep 5 sec, fire in 3 sec, rescheduler to the past" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    scheduler::create_and_insert_one_time_job( & job_id, & error_msg, sched, "test_job", exec_time, std::bind( & task_s, "fire" ) );

    auto res = sched.modify_job( & error_msg, job_id, now );

    if( res )
    {
        std::cout << "ERROR: job was re-scheduled to the past" << std::endl;
    }
    else
    {
        std::cout << "OK: job cannot be re-scheduled to the past: " << error_msg << std::endl;
    }

    THIS_THREAD_SLEEP_SEC( 5 );

    std::cout << name << ": done" << std::endl;
}

void test_08( scheduler::IScheduler & sched )
{
    std::string name( "test_08" );

    std::cout << name << ": sleep 10 sec, fire in 3 sec every second" << std::endl;

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );
    scheduler::Duration period = std::chrono::seconds( 1 );

    scheduler::job_id_t job_id;
    std::string error_msg;

    uint32_t val = 0;

    auto res = scheduler::create_and_insert_periodic_job( & job_id, & error_msg, sched, "test_job", exec_time, period, std::bind( & task_inc, "fire", val ) );

    if( res == false )
    {
        std::cout << "ERROR: cannot create periodic job: " << error_msg << std::endl;

        return;
    }

    THIS_THREAD_SLEEP_SEC( 10 );

    if( val < 7 )
    {
        std::cout << "ERROR: not all jobs fired: val = " << val << std::endl;
    }
    else
    {
        std::cout << "OK: all jobs fired" << std::endl;
    }

    std::cout << name << ": done" << std::endl;
}

int main()
{
    printf( "Hello, world\n" );

    scheduler::Scheduler sched( scheduler::Duration( std::chrono::milliseconds( 1 ) ) );

    sched.run();

    test_01( sched );
    test_02( sched );
    test_03( sched );
    test_04( sched );
    test_05( sched );
    test_06( sched );
    test_07( sched );
    test_08( sched );

    sched.shutdown();

    return 0;
}
