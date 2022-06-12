#include "ComponentRunner.hpp"

#include <chrono>

namespace FluidUi {

    bool ComponentRunner::start_next_computation() {
        if (computation_status != ComputationStatus::WaitingForNextComputation) {
            return false;
        }

        start_or_discard_thread_if_required();

        if (run_asynchronously) {
            // asynchronous computation in another thread
            computation_status = ComputationStatus::IsComputing;
            invoke_computation_start_in_thread = true;
        } else {
            // simple case of synchronous execution
            computation_status = ComputationStatus::IsComputing;
            execute_computation();
            computation_status = ComputationStatus::IsDoneComputing;
        }

        return true;
    }


    void ComponentRunner::start_or_discard_thread_if_required() {
        if (run_asynchronously) {
            if (worker_thread == nullptr) {
                // we should run asynchronous but the worker thread is not running, hence create a new thread
                worker_thread_should_terminate = false;
                worker_thread = std::make_unique<std::thread>(&ComponentRunner::worker_thread_main_loop, this);
            }
        } else {
            if (worker_thread != nullptr) {
                // we should run synchronous but there is a not needed worker thread, hence stop the thread
                worker_thread_should_terminate = true;
                worker_thread->join();
                worker_thread = nullptr;
                worker_thread_should_terminate = false;
            }
        }
    }

    void ComponentRunner::worker_thread_main_loop() {
        while (!worker_thread_should_terminate) {
            // checking if there is work that needs to be done
            if (invoke_computation_start_in_thread) {
                invoke_computation_start_in_thread = false;
                execute_computation();
                computation_status = ComputationStatus::IsDoneComputing;
            } else {
                // there is nothing to do currently
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    ComponentRunner::~ComponentRunner() {
        worker_thread_should_terminate = true;
        worker_thread->join();
        worker_thread = nullptr;
    }

    ComponentRunner::ComputationStatus ComponentRunner::get_status() const {
        return computation_status;
    }
    bool ComponentRunner::reset_runner_on_done() {
        if (computation_status != ComputationStatus::IsDoneComputing) {
            return false;
        }

        computation_status = ComputationStatus::WaitingForNextComputation;
        return true;
    }
    bool ComponentRunner::is_currently_computing() const {
        return computation_status == ComputationStatus::IsComputing;
    }
    bool ComponentRunner::is_done() const {
        return computation_status == ComputationStatus::IsDoneComputing;
    }
    bool ComponentRunner::is_ready() const {
        return computation_status == ComputationStatus::WaitingForNextComputation;
    }
} // namespace FluidUi
