//
// Created by corbi on 25.04.2019.
//

#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include "gtest/gtest.h"

TEST(CubicSplineKernelTest, CompactKernelSupportTest) {
    auto kernel = FluidSolver::CubicSplineKernel();

    EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(2.0f, 0.0f), 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(8.0f, 0.0f), 3.0f), 0.0f);
    EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(3.0f, 16.0f), 5.0f), 0.0f);
    EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(4.0f, 10.0f), 0.8f), 0.0f);
    EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.0f, 3.0f), 2.5f), 0.0f);

}

TEST(CubicSplineKernelTest, SymmetricKernelTest) {
    auto kernel = FluidSolver::CubicSplineKernel();

    for (float t = 0.0f; t <= 10.0f; t += 0.001f) {
        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, t), 8.5f), kernel.GetKernelValue(glm::vec2(-t, -t), 8.5f));

        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.0f), 8.5f),
                        kernel.GetKernelValue(glm::vec2(-t, 0.0f), 8.5f));
        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.0f, t), 6.5f),
                        kernel.GetKernelValue(glm::vec2(0.0f, -t), 6.5f));

        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.2f * t, t), 4.5f),
                        kernel.GetKernelValue(glm::vec2(-0.2f * t, -t), 4.5f));
        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.8f * t, t), 4.5f),
                        kernel.GetKernelValue(glm::vec2(-0.8f * t, -t), 4.5f));

        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.2f * t), 4.5f),
                        kernel.GetKernelValue(glm::vec2(-t, -0.2f * t), 4.5f));
        EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.8f * t), 4.5f),
                        kernel.GetKernelValue(glm::vec2(-t, -0.8f * t), 4.5f));
    }

}

TEST(CubicSplineKernelTest, NonNegativeTest) {
    auto kernel = FluidSolver::CubicSplineKernel();
    for (float t = 0.0f; t <= 10.0f; t += 0.001f) {
        EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, t), 8.5f), 0.0f);

        EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.0f), 8.5f), 0.0f);
        EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.0f, t), 6.5f), 0.0f);

        EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.2f * t, t), 4.5f), 0.0f);
        EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.8f * t, t), 4.5f), 0.0f);

        EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.2f * t), 4.5f), 0.0f);
        EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.8f * t), 4.5f), 0.0f);
    }
}

TEST(CubicSplineKernelDerivativeTest, PointSymmetricTest) {
    auto kernel = FluidSolver::CubicSplineKernel();

    for (float t = 0.0f; t <= 10.0f; t += 0.001f) {

        // x values
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, t), 8.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -t), 8.5f).x);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.0f), 8.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, 0.0f), 8.5f).x);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.0f, t), 6.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(0.0f, -t), 6.5f).x);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.2f * t, t), 4.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-0.2f * t, -t), 4.5f).x);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.8f * t, t), 4.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-0.8f * t, -t), 4.5f).x);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.2f * t), 4.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.2f * t), 4.5f).x);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.8f * t), 4.5f).x,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.8f * t), 4.5f).x);


        // y values
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, t), 8.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -t), 8.5f).y);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.0f), 8.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, 0.0f), 8.5f).y);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.0f, t), 6.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(0.0f, -t), 6.5f).y);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.2f * t, t), 4.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-0.2f * t, -t), 4.5f).y);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.8f * t, t), 4.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-0.8f * t, -t), 4.5f).y);

        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.2f * t), 4.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.2f * t), 4.5f).y);
        EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.8f * t), 4.5f).y,
                        -kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.8f * t), 4.5f).y);
    }
}