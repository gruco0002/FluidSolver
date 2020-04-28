#ifndef FLUIDSOLVER_KERNEL_HPP
#define FLUIDSOLVER_KERNEL_HPP
class FluidSolverWindow;

namespace FluidUI {
    class Kernel {

        public:
            FluidSolverWindow *window = nullptr;

            explicit Kernel(FluidSolverWindow *window);

            void Run(bool *viewKernel);

        private:

            enum KernelType{
                CubicSpline = 0,
                PreCalcCubicSpline = 1,
            };

            KernelType currentType ;

            static const char* names[2];

            bool isInit = false;
            void Init();

            void CheckChange();
    };
}


#endif //FLUIDSOLVER_KERNEL_HPP
