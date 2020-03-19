#ifndef FLUIDSOLVER_IKERNEL_HPP
#define FLUIDSOLVER_IKERNEL_HPP

#include <libraries/glm/glm.hpp>

namespace FluidSolver {


    class IKernel {


    public:

        /**
         * This should return a kernel value evaluated at the given position relative to the origin.
         * @param position Position to evaluate the kernel.
         * @param kernelSupport The kernel support area.
         * @return Kernel value.
         * @note The kernelSupport is the distance from the origin to the location, after which the kernel function evaluates to 0.
         */
        virtual float GetKernelValue(glm::vec2 position, float kernelSupport) = 0;

        /**
         * Helper function calling GetKernelValue with the position := neighborPosition - position
         * @see FluidSolver::IKernel::GetKernelValue
         * @param neighborPosition
         * @param position
         * @param kernelSupport
         * @return
         */
        virtual float GetKernelValue(glm::vec2 neighborPosition, glm::vec2 position, float kernelSupport);

        /**
         * This should return a non reversed kernel derivative value evaluated at the given position relative to the origin.
         * @param position Position to evaluate the kernel.
         * @param kernelSupport The kernel support area.
         * @return Returns a non reversed kernel derivative value.
         * @note The kernelSupport is the distance from the origin to the location, after which the kernel function evaluates to 0.
         */
        virtual glm::vec2 GetKernelDerivativeValue(glm::vec2 position, float kernelSupport) = 0;

        /**
         * Helper function calling GetKernelDerivativeValue with the position := neighborPosition - position
         * @see FluidSolver::IKernel::GetKernelDerivativeValue
         * @param neighborPosition
         * @param position
         * @param kernelSupport 
         * @return
         */
        virtual glm::vec2 GetKernelDerivativeValue(glm::vec2 neighborPosition, glm::vec2 position, float kernelSupport);

        /**
        * Helper function calling -1.0 * GetKernelDerivativeValue with the position := neighborPosition - position
        * @see FluidSolver::IKernel::GetKernelDerivativeValue
        * @param neighborPosition
        * @param position
        * @param kernelSupport
        * @return
        */
        virtual glm::vec2 GetKernelDerivativeReversedValue(glm::vec2 neighborPosition, glm::vec2 position, float kernelSupport);


    };


}

#endif //FLUIDSOLVER_IKERNEL_HPP
