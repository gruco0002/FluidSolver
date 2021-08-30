#ifndef FLUIDSOLVER_FLUIDASSERT_HPP
#define FLUIDSOLVER_FLUIDASSERT_HPP

#include <iostream>

#define FLUID_NAMEOF(name) #name

#ifndef NDEBUG
#define USE_FLUID_ASSERT
#endif

#ifdef _MSC_VER
#define FLUID_BREAK __debugbreak()
#else
#define FLUID_BREAK abort()
#endif


#ifdef USE_FLUID_ASSERT
#define FLUID_ASSERT(condition)                                                                                        \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            std::cerr << "Assertion failed: " << #condition << std::endl;                                              \
            std::cerr << "This happened in " << __FILE__ << ":" << __LINE__ << " in " << __FUNCTION__ << std::endl;    \
            FLUID_BREAK;                                                                                               \
        }                                                                                                              \
    }
#else
#define FLUID_ASSERT(condition)
#endif


#endif // FLUIDSOLVER_FLUIDASSERT_HPP
