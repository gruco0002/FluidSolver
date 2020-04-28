#include <core/interface/Constants.hpp>
#include "PreCalculatedCubicSplineKernel.hpp"

float FluidSolver::PreCalculatedCubicSplineKernel::GetKernelValue(glm::vec2 position) const {

    float length = glm::length(position);

    size_t index = std::round(length / KernelSupport * (float) numberOfSamples);
    if (index >= kernelValues.size())
        return 0.0f;
    return kernelValues[index];
}

glm::vec2
FluidSolver::PreCalculatedCubicSplineKernel::GetKernelDerivativeValue(glm::vec2 position) const {
    size_t  half = numberOfSamples / 2;

    int  xPos = (int)std::round(position.x / KernelSupport * (float)half + (float)half);
    if(xPos < 0 || xPos >= numberOfSamples)
        return glm::vec2 (0.0f);

    int  yPos = (int)std::round(position.y / KernelSupport * (float)half + (float)half);
    if(yPos < 0 || yPos >= numberOfSamples)
        return glm::vec2 (0.0f);

    auto arrayPos = yPos * numberOfSamples + xPos;

    return kernelDerivativeValues[arrayPos];
}

FluidSolver::PreCalculatedCubicSplineKernel::PreCalculatedCubicSplineKernel(float kernelSupport, size_t numberOfSamples)
        : IKernel(kernelSupport), numberOfSamples(numberOfSamples) {

    PrecalculateValues();

}

void FluidSolver::PreCalculatedCubicSplineKernel::PrecalculateValues() {
    PrecalculateKernel();
    PrecalculateDerivative();
}

FluidSolver::IKernel *FluidSolver::PreCalculatedCubicSplineKernel::CreateCopy(float kernelSupport) {
    return new PreCalculatedCubicSplineKernel(kernelSupport, numberOfSamples);
}

void FluidSolver::PreCalculatedCubicSplineKernel::PrecalculateKernel() {

    kernelValues.clear();
    for (size_t i = 0; i < numberOfSamples; i++) {

        float length = i * KernelSupport / (float) numberOfSamples;


        float h = KernelSupport / 2.0f;
        float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
        float q = length / h;


        float ret = 0.0f;
        if (q < 1.0f) {
            ret = std::pow(2.0f - q, 3.0f) - 4.0f * std::pow(1.0f - q, 3.0f);
        } else if (q < 2.0f) {
            ret = std::pow(2.0f - q, 3.0f);
        } else if (q >= 2.0f) {
            ret = 0.0f;
        }
        float result = alpha * ret;

        kernelValues.push_back(result);
    }
}

void FluidSolver::PreCalculatedCubicSplineKernel::PrecalculateDerivative() {

    kernelDerivativeValues.clear();

    size_t  half = numberOfSamples / 2;

    for(size_t y = 0; y < numberOfSamples; y++){
        for(size_t x = 0; x < numberOfSamples; x++){

            float xPos = (float)((int)x - (int)half) * KernelSupport / (float)half;
            float yPos = (float)((int)y - (int)half) * KernelSupport/ (float)half;

            //auto arrayPos = y * numberOfSamples + x;
            glm::vec2 position = glm::vec2(xPos, yPos);

            float h = KernelSupport / 2.0f;
            float alpha = 5.0f / (14.0f * FS_PI * std::pow(h, 2.0f));
            float q = glm::length(position) / h;

            glm::vec2 pre = glm::vec2(0.0f);
            if (glm::length(position) > std::numeric_limits<float>::epsilon())
                pre = h * position / glm::length(position);


            glm::vec2 ret = glm::vec2(0.0f);
            // this is the reversed implementation
            if (q < 1.0f) {
                ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f) + 12.0f * std::pow(1.0f - q, 2.0f));
            } else if (q < 2.0f) {
                ret = pre * (-3.0f * std::pow(2.0f - q, 2.0f));
            } else if (q >= 2.0f) {
                ret = glm::vec2(0.0f);
            }

            // reverse the reversed implementation, since this function should return a non reversed kernel derivative value
            auto result = alpha * ret * -1.0f;

            kernelDerivativeValues.push_back(result);
        }
    }



}
