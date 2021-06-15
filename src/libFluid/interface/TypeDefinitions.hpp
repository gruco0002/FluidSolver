#ifndef FLUIDSOLVER_TYPEDEFINITIONS_HPP
#define FLUIDSOLVER_TYPEDEFINITIONS_HPP

#include <stdexcept>

namespace FluidSolver
{


#ifdef WIN32
    typedef int64_t particleIndex_t;
    typedef int64_t particleID_t;
#endif
#ifndef WIN32
    typedef size_t particleIndex_t;
    typedef size_t particleID_t;
#endif


    typedef size_t particleAmount_t;
} // namespace FluidSolver

#endif // FLUIDSOLVER_TYPEDEFINITIONS_HPP
