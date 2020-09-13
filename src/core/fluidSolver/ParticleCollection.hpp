#ifndef FLUIDSOLVER_PARTICLECOLLECTION_HPP
#define FLUIDSOLVER_PARTICLECOLLECTION_HPP

#include <vector>
#include <functional>
#include "Particle.hpp"

namespace FluidSolver {

    class ParticleCollection {
    private:
        size_t internal_size = 0;
        std::vector<void *> data;
        std::vector<void *> data_ptr;
        std::vector<std::function<void(size_t)>> internal_resize_calls;
        std::vector<std::function<void(size_t, size_t)>> internal_swap_calls;
        std::vector<std::function<void()>> internal_delete;

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

    public:
        template<typename Component>
        void add_type() {
            auto typeId = family::type<Component>();
            if (data.size() <= typeId) {
                data.resize(typeId + 1, nullptr);
                data_ptr.resize(typeId + 1, nullptr);
            }
            if (data[typeId] == nullptr) {
                data[typeId] = new std::vector<Component>(internal_size);
                data_ptr[typeId] = ((std::vector<Component> *) data[typeId])->data();
                internal_resize_calls.push_back([&, typeId](size_t new_size) {
                    ((std::vector<Component> *) data[typeId])->resize(new_size);
                    data_ptr[typeId] = ((std::vector<Component> *) data[typeId])->data();
                });
                internal_swap_calls.push_back([&, typeId](size_t i, size_t j) {
                    std::swap(((std::vector<Component> *) data[typeId])[i],
                              ((std::vector<Component> *) data[typeId])[j]);
                });
                internal_delete.push_back([&, typeId]() {
                    delete ((std::vector<Component> *) data[typeId]);
                    data[typeId] = nullptr;
                });
            }
        }

        template<typename Component>
        bool is_type_present() const {
            auto typeId = family::type<Component>();
            if (data.size() <= typeId) {
                return false;
            }
            if (data[typeId] == nullptr) {
                return false;
            }
            return true;
        }

        size_t add() {
            auto index = internal_size++;
            for (auto &fn : internal_resize_calls) {
                fn(internal_size);
            }
            return index;
        }

        void resize(size_t new_size) {
            internal_size = new_size;
            for (auto &fn : internal_resize_calls) {
                fn(internal_size);
            }
        }

        size_t size() const { return internal_size; }

        void swap(size_t i, size_t j) {
            for (auto &fn : internal_swap_calls) {
                fn(i, j);
            }
        }

        template<typename Component>
        Component &get(size_t id) {
            return ((Component *) data_ptr[family::type<Component>()])[id];
        }

        void clear() {
            resize(0);
        }

        ~ParticleCollection() {
            for (auto &fn : internal_delete) {
                fn();
            }
        }

        ParticleCollection() = default;

        ParticleCollection(const ParticleCollection&) = delete;
    };
}

#endif //FLUIDSOLVER_PARTICLECOLLECTION_HPP
