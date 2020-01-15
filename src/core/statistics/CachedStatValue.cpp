//
// Created by corbi on 09.01.2020.
//

#include "CachedStatValue.hpp"

FluidSolver::CachedStatValue::CachedStatValue(FluidSolver::StatValue *statValue) {
    this->Name = statValue->Name;
    this->Description = statValue->Description;
    this->ValueType = statValue->ValueType;

    // copy values
    this->UIntValue = statValue->UIntValue;
    this->FloatValue = statValue->FloatValue;

    delete statValue;
}

void FluidSolver::CachedStatValue::Resize(size_t newCacheSize) {
    FloatCache.resize(newCacheSize);
    UIntCache.resize(newCacheSize);
}

size_t FluidSolver::CachedStatValue::GetSize() {
    return FloatCache.size();
}

void FluidSolver::CachedStatValue::SetValue(size_t i) {
    if (ValueType == StatValueTypeFloat) {
        FloatCache[i] = FloatValue;
        UIntCache[i] = (uint32_t) FloatValue;
    } else {
        UIntCache[i] = UIntValue;
        FloatCache[i] = (float) UIntValue;
    }
}

FluidSolver::CachedStatValue::CachedStatValue(const std::string &name, const std::string &description,
                                              FluidSolver::StatValue::StatValueType valueType) : StatValue(name,
                                                                                                           description,
                                                                                                           valueType) {

}
