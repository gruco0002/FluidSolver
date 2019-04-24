//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_IKERNEL_HPP
#define FLUIDSOLVER_IKERNEL_HPP

#include <libraries/glm/glm.hpp>

namespace FluidSolver {


    class IKernel {


    public:

        virtual float GetKernelValue(glm::vec2 position, float particleSize) = 0;

        virtual float GetKernelValue(glm::vec2 origin, glm::vec2 position, float particleSize);


    };


}

#endif //FLUIDSOLVER_IKERNEL_HPP
