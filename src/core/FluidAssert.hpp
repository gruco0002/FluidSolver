#ifndef FLUIDSOLVER_FLUIDASSERT_HPP
#define FLUIDSOLVER_FLUIDASSERT_HPP

#include <iostream>

#ifndef NDEBUG
    #define USE_FLUID_ASSERT
#endif

#ifdef USE_FLUID_ASSERT
    #define FLUID_ASSERT(condition)                                     \
        {                                                               \
            if (!(condition)) {                                         \
                std::cerr << "Assertion failed: " << #condition << std::endl; \
                std::cerr << "This happened in " << __FILE__ << ":" << __LINE__ << " in " << __FUNCTION__ << std::endl; \
                abort();                                          \
            }                                                            \
        }
#else
    #define FLUID_ASSERT(condition) {(condition);}
#endif


#endif //FLUIDSOLVER_FLUIDASSERT_HPP
