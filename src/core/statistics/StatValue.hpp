#ifndef FLUIDSOLVER_STATVALUE_HPP
#define FLUIDSOLVER_STATVALUE_HPP

#include <string>

namespace FluidSolver {


    class StatValue {
    protected:
        StatValue() = default;

    public:

        enum StatValueType {
            StatValueTypeFloat,
            StatValueTypeUInt
        };

        virtual ~StatValue();

        StatValue(const std::string &name, const std::string &description, StatValueType valueType);

        std::string Name;
        std::string Description;

        StatValueType ValueType;

        float FloatValue = 0.0f;
        uint32_t UIntValue = 0;


        std::string ToString();

        float GetValueAsFloat();

        uint32_t GetValueAsUInt();

        StatValue &operator+=(const float &rhs);

        StatValue &operator-=(const float &rhs);

        StatValue &operator*=(const float &rhs);

        StatValue &operator/=(const float &rhs);

        void Max(const float &rhs);

        void Min(const float &rhs);

        StatValue &operator+=(const uint32_t &rhs);

        StatValue &operator-=(const uint32_t &rhs);

        StatValue &operator*=(const uint32_t &rhs);

        StatValue &operator/=(const uint32_t &rhs);

        void Max(const uint32_t &rhs);

        void Min(const uint32_t &rhs);

        void Set(float value);

        void Set(uint32_t value);


    };
}


#endif //FLUIDSOLVER_STATVALUE_HPP
