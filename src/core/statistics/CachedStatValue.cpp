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
    if (ValueType == StatValueTypeFloat) {
        FloatCache.resize(newCacheSize);
        UIntCache.resize(0);
    } else {
        FloatCache.resize(0);
        UIntCache.resize(newCacheSize);
    }
}

size_t FluidSolver::CachedStatValue::GetSize() {
    if (ValueType == StatValueTypeFloat) {
        return FloatCache.size();
    } else {
        return UIntCache.size();
    }
}

void FluidSolver::CachedStatValue::SetValue(size_t i) {
    if (ValueType == StatValueTypeFloat) {
        FloatCache[i] = FloatValue;
    } else {
        UIntCache[i] = UIntValue;
    }
}

FluidSolver::CachedStatValue::CachedStatValue(const std::string &name, const std::string &description,
                                              FluidSolver::StatValue::StatValueType valueType) : StatValue(name,
                                                                                                           description,
                                                                                                           valueType) {

}
