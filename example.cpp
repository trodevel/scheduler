#include <cstdio>

#include <sstream>          // std::ostringstream
#include <iostream>         // std::cout
#include <functional>       // std::bind

#include "utils/mutex_helper.h" // MUTEX_SCOPE_LOCK

#include "scheduler.h"          // Scheduler
#include "onetime_job_aux.h"    // OneTimeJob

void task( unsigned a )
{
    std::ostringstream os;

    os << "task_" << a << "\n";

    std::cout << os.str();
}

void test_01( scheduler::IScheduler & sched )
{
    std::cout << "test_01" << std::endl;

    std::bind( &task, 1 );

    auto now = std::chrono::system_clock::now();

    scheduler::Time exec_time = now + std::chrono::seconds( 3 );

    auto job = scheduler::create_one_time_job( "test_job", exec_time, std::bind( & task, 1 ) );

    scheduler::job_id_t job_id;
    std::string error_msg;

    sched.insert_job( & job_id, & error_msg, job );

    THIS_THREAD_SLEEP_SEC( 5 );
}

int main()
{
    printf( "Hello, world\n" );

    scheduler::Scheduler sched( scheduler::Duration( std::chrono::milliseconds( 1 ) ) );

    sched.run();

    test_01( sched );

    sched.shutdown();

    return 0;
}
