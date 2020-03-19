#include <algorithm>
#include "StatValue.hpp"


FluidSolver::StatValue::StatValue(const std::string &name, const std::string &description,
                                  FluidSolver::StatValue::StatValueType valueType) : Name(name),
                                                                                     Description(description),
                                                                                     ValueType(valueType) {}

std::string FluidSolver::StatValue::ToString() {
    if (ValueType == StatValueTypeFloat) {
        return std::to_string(FloatValue);
    } else {
        return std::to_string(UIntValue);
    }

}

float FluidSolver::StatValue::GetValueAsFloat() {
    if (ValueType == StatValueTypeFloat) {
        return FloatValue;
    } else {
        return (float) UIntValue;
    }
}

uint32_t FluidSolver::StatValue::GetValueAsUInt() {
    if (ValueType == StatValueTypeFloat) {
        return (uint32_t) FloatValue;
    } else {
        return UIntValue;
    }
}

FluidSolver::StatValue &FluidSolver::StatValue::operator+=(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue += rhs;
    } else {
        UIntValue += rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator-=(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue -= rhs;
    } else {
        UIntValue -= rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator*=(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue *= rhs;
    } else {
        UIntValue *= rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator/=(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue /= rhs;
    } else {
        UIntValue /= rhs;
    }
    return *this;
}

void FluidSolver::StatValue::Max(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = std::max(FloatValue, rhs);
    } else {
        UIntValue = std::max(UIntValue, (uint32_t) rhs);
    }
}

void FluidSolver::StatValue::Min(const float &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = std::min(FloatValue, rhs);
    } else {
        UIntValue = std::min(UIntValue, (uint32_t) rhs);
    }
}

FluidSolver::StatValue &FluidSolver::StatValue::operator+=(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue += rhs;
    } else {
        UIntValue += rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator-=(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue -= rhs;
    } else {
        UIntValue -= rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator*=(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue *= rhs;
    } else {
        UIntValue *= rhs;
    }
    return *this;
}

FluidSolver::StatValue &FluidSolver::StatValue::operator/=(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue /= rhs;
    } else {
        UIntValue /= rhs;
    }
    return *this;
}

void FluidSolver::StatValue::Max(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = std::max(FloatValue, (float) rhs);
    } else {
        UIntValue = std::max(UIntValue, rhs);
    }

}

void FluidSolver::StatValue::Min(const uint32_t &rhs) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = std::min(FloatValue, (float) rhs);
    } else {
        UIntValue = std::min(UIntValue, rhs);
    }
}

void FluidSolver::StatValue::Set(float value) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = value;
    } else {
        UIntValue = value;
    }
}

void FluidSolver::StatValue::Set(uint32_t value) {
    if (ValueType == StatValueTypeFloat) {
        FloatValue = value;
    } else {
        UIntValue = value;
    }
}

FluidSolver::StatValue::~StatValue() {

}
