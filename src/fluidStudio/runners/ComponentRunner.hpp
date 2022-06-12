#pragma once

#include <thread>

namespace FluidUi {

    class ComponentRunner {

      public:

        enum class ComputationStatus {
            IsComputing,
            IsDoneComputing,
            WaitingForNextComputation
        };

        ComputationStatus get_status() const;

        bool start_next_computation();

        bool reset_runner_on_done();

        virtual ~ComponentRunner();

        bool run_asynchronously = false;

        bool is_currently_computing() const;

        bool is_done() const;

        bool is_ready() const;

      protected:

        virtual void execute_computation() = 0;

      private:

        ComputationStatus computation_status = ComputationStatus::WaitingForNextComputation;

        std::unique_ptr<std::thread> worker_thread;

        void start_or_discard_thread_if_required();

        bool invoke_computation_start_in_thread = false;

        void worker_thread_main_loop();

        bool worker_thread_should_terminate = false;



    };
}
