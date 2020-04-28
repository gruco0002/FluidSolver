#ifndef FLUIDSOLVER_IKERNEL_HPP
#define FLUIDSOLVER_IKERNEL_HPP

#include <libraries/glm/glm.hpp>

namespace FluidSolver {


    class IKernel {

    public:

        /**
         * Creates a new instance of a kernel with the given kernel support.
         * @param kernelSupport The kernelSupport is the distance from the origin to the location, after which the
         * kernel function evaluates to 0.
         */
        explicit IKernel(float kernelSupport);

        /**
         * This should return a kernel value evaluated at the given position relative to the origin.
         * @param position Position to evaluate the kernel.
         * @return Kernel value.
         */
        virtual float GetKernelValue(glm::vec2 position) const = 0;

        /**
         * Helper function calling GetKernelValue with the position := neighborPosition - position
         * @see FluidSolver::IKernel::GetKernelValue
         * @param neighborPosition
         * @param position
         * @return
         */
        virtual float GetKernelValue(glm::vec2 neighborPosition, glm::vec2 position) const;

        /**
         * This should return a non reversed kernel derivative value evaluated at the given position relative to the origin.
         * @param position Position to evaluate the kernel.
         * @return Returns a non reversed kernel derivative value.
         */
        virtual glm::vec2 GetKernelDerivativeValue(glm::vec2 position) const = 0;

        /**
         * Helper function calling GetKernelDerivativeValue with the position := neighborPosition - position
         * @see FluidSolver::IKernel::GetKernelDerivativeValue
         * @param neighborPosition
         * @param position
         * @return
         */
        virtual glm::vec2 GetKernelDerivativeValue(glm::vec2 neighborPosition, glm::vec2 position) const;

        /**
        * Helper function calling -1.0 * GetKernelDerivativeValue with the position := neighborPosition - position
        * @see FluidSolver::IKernel::GetKernelDerivativeValue
        * @param neighborPosition
        * @param position
        * @return
        */
        virtual glm::vec2 GetKernelDerivativeReversedValue(glm::vec2 neighborPosition, glm::vec2 position) const;


        virtual ~IKernel();

        float GetKernelSupport() const;

        virtual IKernel* CreateCopy(float kernelSupport) = 0;

    protected:
        float KernelSupport;


    };


}

#endif //FLUIDSOLVER_IKERNEL_HPP
