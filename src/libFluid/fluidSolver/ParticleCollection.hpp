#pragma once

#include "LibFluidAssert.hpp"
#include "Particle.hpp"

#include <functional>
#include <vector>

namespace LibFluid
{

    class ParticleCollection
    {
      private:
        size_t internal_size = 0;
        std::vector<void *> data;
        std::vector<void *> data_ptr;
        std::vector<std::function<void(ParticleCollection *, size_t)>> internal_resize_calls;
        std::vector<std::function<void(ParticleCollection *, size_t, size_t)>> internal_swap_calls;
        std::vector<std::function<void(ParticleCollection *)>> internal_delete;
        std::vector<std::function<void(const ParticleCollection *from, ParticleCollection *to)>> internal_copy_data;

        class family
        {
            static std::size_t identifier() noexcept
            {
                static std::size_t value = 0;
                return value++;
            }

          public:
            template <typename> static std::size_t type() noexcept
            {
                static const std::size_t value = identifier();
                return value;
            }
        };

      public:
        template <typename Component> void add_type()
        {
            auto typeId = family::type<Component>();
            if (data.size() <= typeId)
            {
                data.resize(typeId + 1, nullptr);
                data_ptr.resize(typeId + 1, nullptr);
            }
            if (data[typeId] == nullptr)
            {
                data[typeId] = new std::vector<Component>(internal_size);
                data_ptr[typeId] = ((std::vector<Component> *)data[typeId])->data();
                internal_resize_calls.push_back([typeId](ParticleCollection *c, size_t new_size) {
                    ((std::vector<Component> *)c->data[typeId])->resize(new_size);
                    c->data_ptr[typeId] = ((std::vector<Component> *)c->data[typeId])->data();
                });
                internal_swap_calls.push_back([typeId](ParticleCollection *c, size_t i, size_t j) {
                    std::swap((*((std::vector<Component> *)c->data[typeId]))[i],
                              (*((std::vector<Component> *)c->data[typeId]))[j]);
                });
                internal_delete.push_back([typeId](ParticleCollection *c) {
                    delete ((std::vector<Component> *)c->data[typeId]);
                    c->data[typeId] = nullptr;
                });
                internal_copy_data.push_back([typeId](const ParticleCollection *from, ParticleCollection *to) {
                    if (!to->is_type_present<Component>())
                        to->add_type<Component>();
                    auto ptr = (std::vector<Component> *)from->data[typeId];
                    ((std::vector<Component> *)to->data[typeId])->assign(ptr->begin(), ptr->end());
                    to->data_ptr[typeId] = ((std::vector<Component> *)to->data[typeId])->data();
                });
            }
        }

        template <typename Component> bool is_type_present() const
        {
            auto typeId = family::type<Component>();
            if (data.size() <= typeId)
            {
                return false;
            }
            if (data[typeId] == nullptr)
            {
                return false;
            }
            return true;
        }

        size_t add()
        {
            auto index = internal_size++;
            for (auto &fn : internal_resize_calls)
            {
                fn(this, internal_size);
            }
            return index;
        }

        void resize(size_t new_size)
        {
            internal_size = new_size;
            for (auto &fn : internal_resize_calls)
            {
                fn(this, internal_size);
            }
        }

        size_t size() const
        {
            return internal_size;
        }

        void swap(size_t i, size_t j)
        {
            FLUID_ASSERT(i < internal_size);
            FLUID_ASSERT(j < internal_size);
            for (auto &fn : internal_swap_calls)
            {
                fn(this, i, j);
            }
        }

        template <typename Component> Component &get(size_t id)
        {
            return ((Component *)data_ptr[family::type<Component>()])[id];
        }

        void clear()
        {
            resize(0);
        }

        ~ParticleCollection()
        {
            for (auto &fn : internal_delete)
            {
                fn(this);
            }
        }

        ParticleCollection() = default;

        ParticleCollection(const ParticleCollection &o)
        {
            // copy constructor

            this->internal_size = o.internal_size;

            // calls can be copied, since the typeid will not change and the collection instance is given as argument
            this->internal_resize_calls = o.internal_resize_calls;
            this->internal_swap_calls = o.internal_swap_calls;
            this->internal_delete = o.internal_delete;
            this->internal_copy_data = o.internal_copy_data;

            // copy the data
            for (auto &fn : o.internal_copy_data)
            {
                fn(&o, this);
            }
        }

        ParticleCollection(ParticleCollection &&m) noexcept
        {
            // move constructor
            this->data = m.data;
            this->data_ptr = m.data_ptr;
            m.data.clear();
            m.data_ptr.clear();

            this->internal_resize_calls = m.internal_resize_calls;
            m.internal_resize_calls.clear(); // this prevents deletion
            this->internal_swap_calls = m.internal_swap_calls;
            m.internal_swap_calls.clear(); // this prevents deletion
            this->internal_delete = m.internal_delete;
            m.internal_delete.clear(); // this prevents deletion
            this->internal_copy_data = m.internal_copy_data;
            m.internal_copy_data.clear();

            this->internal_size = m.internal_size;
        }
    };
} // namespace LibFluid
