#pragma once

#include <unordered_map>

namespace FluidSolver::Helper
{

    template <typename _Key, typename _Value, typename _Hash = std::hash<_Key>>
    class ProtectedUnorderedMap : public std::unordered_map<_Key, _Value, _Hash> {

      public:
        bool auto_initialize_protection_enabled = false;
        _Value fallback_empty_value;

        inline virtual void reinitalize_fallback_value()
        {
            fallback_empty_value = {};
        }


        _Value& operator[](const _Key& _key)
        {
            auto casted = static_cast<std::unordered_map<_Key, _Value, _Hash>*>(this);
            if (!auto_initialize_protection_enabled)
            {
                return (*casted)[_key];
            }
            else
            {
                if (casted->find(_key) != casted->end())
                {
                    return (*casted)[_key];
                }
                else
                {
                    reinitalize_fallback_value();
                    return fallback_empty_value;
                }
            }
        }
    };


} // namespace FluidSolver::Helper
