#pragma once

#include <memory>

namespace LibFluid::Serialization
{

    template <typename T, typename U> bool dynamic_pointer_is(const std::shared_ptr<U> &ptr)
    {
        return std::dynamic_pointer_cast<T>(ptr) != nullptr;
    }

} // namespace LibFluid::Serialization
