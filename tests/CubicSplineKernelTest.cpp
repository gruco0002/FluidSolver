#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include "gtest/gtest.h"

TEST(CubicSplineKernelTest, CompactKernelSupportTest) {
	auto kernel = FluidSolver::CubicSplineKernel();

	kernel.kernel_support = 1.0f;
	EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(2.0f, 0.0f)), 0.0f);

	kernel.kernel_support = 3.0f;
	EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(8.0f, 0.0f)), 0.0f);

	kernel.kernel_support = 5.0f;
	EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(3.0f, 16.0f)), 0.0f);

	kernel.kernel_support = 0.8f;
	EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(4.0f, 10.0f)), 0.0f);

	kernel.kernel_support = 2.5f;
	EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.0f, 3.0f)), 0.0f);

}

TEST(CubicSplineKernelTest, SymmetricKernelTest) {
	auto kernel = FluidSolver::CubicSplineKernel();

	for (float t = 0.0f; t <= 10.0f; t += 0.001f) {
		kernel.kernel_support = 8.5f;
		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, t)), kernel.GetKernelValue(glm::vec2(-t, -t)));

		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.0f)),
			kernel.GetKernelValue(glm::vec2(-t, 0.0f)));

		kernel.kernel_support = 6.5f;
		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.0f, t)),
			kernel.GetKernelValue(glm::vec2(0.0f, -t)));

		kernel.kernel_support = 4.5f;
		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.2f * t, t)),
			kernel.GetKernelValue(glm::vec2(-0.2f * t, -t)));
		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(0.8f * t, t)),
			kernel.GetKernelValue(glm::vec2(-0.8f * t, -t)));

		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.2f * t)),
			kernel.GetKernelValue(glm::vec2(-t, -0.2f * t)));
		EXPECT_FLOAT_EQ(kernel.GetKernelValue(glm::vec2(t, 0.8f * t)),
			kernel.GetKernelValue(glm::vec2(-t, -0.8f * t)));
	}

}

TEST(CubicSplineKernelTest, NonNegativeTest) {
	auto kernel = FluidSolver::CubicSplineKernel();
	for (float t = 0.0f; t <= 10.0f; t += 0.001f) {
		kernel.kernel_support = 8.5f;
		EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, t)), 0.0f);

		EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.0f)), 0.0f);

		kernel.kernel_support = 6.5f;
		EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.0f, t)), 0.0f);

		kernel.kernel_support = 4.5f;
		EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.2f * t, t)), 0.0f);
		EXPECT_GE(kernel.GetKernelValue(glm::vec2(0.8f * t, t)), 0.0f);

		EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.2f * t)), 0.0f);
		EXPECT_GE(kernel.GetKernelValue(glm::vec2(t, 0.8f * t)), 0.0f);
	}
}

TEST(CubicSplineKernelDerivativeTest, PointSymmetricTest) {
	auto kernel = FluidSolver::CubicSplineKernel();

	for (float t = 0.0f; t <= 10.0f; t += 0.001f) {

		{
			kernel.kernel_support = 8.5f;
			// x values
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -t)).x);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.0f)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, 0.0f)).x);
			// y values
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -t)).y);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.0f)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, 0.0f)).y);
		}

		{
			kernel.kernel_support = 6.5f;
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.0f, t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(0.0f, -t)).x);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.0f, t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(0.0f, -t)).y);
		}

		{
			kernel.kernel_support = 4.5f;
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.2f * t, t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-0.2f * t, -t)).x);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.8f * t, t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-0.8f * t, -t)).x);

			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.2f * t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.2f * t)).x);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.8f * t)).x,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.8f * t)).x);

			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.2f * t, t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-0.2f * t, -t)).y);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(0.8f * t, t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-0.8f * t, -t)).y);

			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.2f * t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.2f * t)).y);
			EXPECT_FLOAT_EQ(kernel.GetKernelDerivativeValue(glm::vec2(t, 0.8f * t)).y,
				-kernel.GetKernelDerivativeValue(glm::vec2(-t, -0.8f * t)).y);
		}
	}
}