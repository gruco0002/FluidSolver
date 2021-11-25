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

    #define FLUID_ASSERT_WITH_MESSAGE(condition, message)                                                               \
        {                                                                                                               \
            if (!(condition)) {                                                                                         \
                std::cerr << "Assertion failed: " << message << std::endl;                                              \
                std::cerr << "Failed condition: " << #condition << std::endl;                                           \
                std::cerr << "This happened in " << __FILE__ << ":" << __LINE__ << " in " << __FUNCTION__ << std::endl; \
                FLUID_BREAK;                                                                                            \
            }                                                                                                           \
        }


    #define FLUID_ASSERT_WITHOUT_MESSAGE(condition)                                                                     \
        {                                                                                                               \
            if (!(condition)) {                                                                                         \
                std::cerr << "Assertion failed: " << #condition << std::endl;                                           \
                std::cerr << "This happened in " << __FILE__ << ":" << __LINE__ << " in " << __FUNCTION__ << std::endl; \
                FLUID_BREAK;                                                                                            \
            }                                                                                                           \
        }


#else
    #define FLUID_ASSERT_WITH_MESSAGE(condition, message)
    #define FLUID_ASSERT_WITHOUT_MESSAGE(condition)
#endif

#define GET_MACRO(_1, _2, NAME, ...) NAME
#define FLUID_ASSERT(...)                                                           \
    GET_MACRO(__VA_ARGS__, FLUID_ASSERT_WITH_MESSAGE, FLUID_ASSERT_WITHOUT_MESSAGE) \
    (__VA_ARGS__)


#endif // FLUIDSOLVER_FLUIDASSERT_HPP
