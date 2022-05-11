#pragma once

#include <thread>

namespace FluidUi {

    class ComponentRunner {

      public:

        bool is_currently_computing() const;

        bool start_next_computation();

        virtual ~ComponentRunner();

        bool run_asynchronously = false;

      protected:

        virtual void execute_computation() = 0;

      private:

        bool currently_computing = false;

        std::unique_ptr<std::thread> worker_thread;

        void start_or_discard_thread_if_required();

        bool invoke_computation_start_in_thread = false;

        void worker_thread_main_loop();

        bool worker_thread_should_terminate = false;



    };
}
