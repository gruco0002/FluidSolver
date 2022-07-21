#pragma once

#include <unordered_map>

namespace LibFluid::Helper
{

    template <typename _Key, typename _Value, typename _Hash = std::hash<_Key>> class ProtectedUnorderedMap {

      private:
        std::unordered_map<_Key, _Value, _Hash> data;

      public:
        bool auto_initialize_protection_enabled = false;
        _Value fallback_empty_value;

        inline virtual void reinitalize_fallback_value()
        {
            fallback_empty_value = {};
        }


        _Value& operator[](const _Key& _key)
        {

            if (!auto_initialize_protection_enabled)
            {
                return data[_key];
            }
            else
            {
                if (data.find(_key) != data.end())
                {
                    return data[_key];
                }
                else
                {
                    reinitalize_fallback_value();
                    return fallback_empty_value;
                }
            }
        }

        void clear()
        {
            data.clear();
        }
    };


} // namespace FluidSolver::Helper
