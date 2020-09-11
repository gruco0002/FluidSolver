#ifndef FLUIDSOLVER_STATISTICS_HPP
#define FLUIDSOLVER_STATISTICS_HPP

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include "core/FluidAssert.hpp"

namespace FluidSolver {
    class Statistics {

    public:
        using timepoint = std::chrono::time_point<std::chrono::system_clock>;
        struct Time {
            float simulation_time;
            timepoint system_time;
        };

    private:
        Statistics() = default;

        static Statistics internal_instance;

        class family {
            static std::size_t identifier() noexcept {
                static std::size_t value = 0;
                return value++;
            }

        public:

            template<typename>
            static std::size_t type() noexcept {
                static const std::size_t value = identifier();
                return value;
            }
        };

        std::vector<void *> data;
        std::vector<std::string> names;
        std::vector<std::function<void()>> internal_delete;


    public:

        float simulation_time = 0.0f;

        template<typename T>
        void add_type(const std::string &name) {
            size_t id = family::type<T>();
            if (id >= data.size()) {
                data.resize(id + 1, nullptr);
                names.resize(id + 1);
            }
            if (data[id] == nullptr) {
                data[id] = new std::vector<std::pair<Time, T>>();
                names[id] = name;
                internal_delete.push_back([&, id]() {
                    delete ((T *) data[id]);
                    data[id] = nullptr;
                });
            }
        }

        template<typename T>
        void push_back(const T &stat) {
            size_t id = family::type<T>();
            FLUID_ASSERT(data.size() > id);
            FLUID_ASSERT(data[id] != nullptr);
            Time t;
            t.simulation_time = simulation_time;
            t.system_time = std::chrono::system_clock::now();
            ((std::vector<std::pair<Time, T>> *) data[id])->push_back({t, stat});
        }


        static Statistics& get()  {
            return internal_instance;
        }

        ~Statistics() {
            for (auto &fn: internal_delete) {
                fn();
            }
        }

    };
}


#endif //FLUIDSOLVER_STATISTICS_HPP
