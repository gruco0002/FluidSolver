#ifndef FLUIDSOLVER_SENSORDATASTORAGE_HPP
#define FLUIDSOLVER_SENSORDATASTORAGE_HPP

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <map>

#include "core/FluidAssert.hpp"


namespace FluidSolver {
    class SensorDataStorage {

    public:
        using timepoint = std::chrono::time_point<std::chrono::system_clock>;
        struct Time {
            float simulation_time;
            timepoint system_time;
        };


    private:

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


        struct TypeStorage {
            void *ptr;
            size_t type_id;
            size_t sensor_id;
            std::string name;

            std::function<void()> internal_delete;
            std::function<void()> internal_clear;
        };

        std::map<std::pair<size_t, size_t>, TypeStorage> data;


    public:

        SensorDataStorage() = default;

        float simulation_time = 0.0f;

        template<typename T>
        void add_type(const std::string &name, size_t sensor_id = 0) {
            size_t type_id = family::type<T>();

            if (data.find({type_id, sensor_id}) == data.end()) {
                auto vec = new std::vector<std::pair<Time, T>>();

                data[{type_id, sensor_id}] =
                        {
                                vec,
                                type_id,
                                sensor_id,
                                name,
                                [&, type_id, sensor_id]() {
                                    delete ((std::vector<std::pair<Time, T>> *) data[{type_id, sensor_id}].ptr);
                                    data[{type_id, sensor_id}].ptr = nullptr;
                                },
                                [&, type_id, sensor_id]() {
                                    ((std::vector<std::pair<Time, T>> *) data[{type_id, sensor_id}].ptr)->clear();
                                }
                        };
            }

        }

        template<typename T>
        void push_back(const T &stat, size_t sensor_id = 0) {
            size_t type_id = family::type<T>();
            FLUID_ASSERT(data.find({type_id, sensor_id}) != data.end())
            FLUID_ASSERT((data[{type_id, sensor_id}].ptr) != nullptr)
            Time t;
            t.simulation_time = simulation_time;
            t.system_time = std::chrono::system_clock::now();
            ((std::vector<std::pair<Time, T>> *) data[{type_id, sensor_id}].ptr)->push_back({t, stat});
        }

        template<typename T>
        size_t size(size_t sensor_id = 0) const {
            size_t type_id = family::type<T>();
            FLUID_ASSERT(data.find({type_id, sensor_id}) != data.end())
            FLUID_ASSERT((data[{type_id, sensor_id}].ptr) != nullptr)
            return ((std::vector<std::pair<Time, T>> *) data[{type_id, sensor_id}].ptr)->size();
        }

        void clear() {
            for (auto &[key, value]: data) {
                value.internal_clear();
            }
            simulation_time = 0.0f;
        }

        template<typename T>
        const std::vector<std::pair<Time, T>> &get(size_t sensor_id = 0) const {
            size_t type_id = family::type<T>();
            FLUID_ASSERT(data.find({type_id, sensor_id}) != data.end())
            FLUID_ASSERT((data[{type_id, sensor_id}].ptr) != nullptr)
            return *((std::vector<std::pair<Time, T>> *) data[{type_id, sensor_id}].ptr);
        }

        template<typename T>
        bool is_type_present(size_t sensor_id = 0) const {
            size_t type_id = family::type<T>();
            if (data.find({type_id, sensor_id}) == data.end()) return false;
            if (data[{type_id, sensor_id}].ptr == nullptr) return false;
            return true;
        }

        template<typename T>
        void rename(const std::string &name, size_t sensor_id = 0) {
            size_t type_id = family::type<T>();
            FLUID_ASSERT(data.find({type_id, sensor_id}) != data.end())
            data[{type_id, sensor_id}].name = name;
        }

        ~SensorDataStorage() {
            for (auto &[key, value]: data) {
                value.internal_delete();
            }
        }

    };
}


#endif //FLUIDSOLVER_SENSORDATASTORAGE_HPP
