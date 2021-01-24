#include "SensorWriter.hpp"

void FluidSolver::SensorWriter::push_back_header(const std::string& header, size_t dimensionality)
{
	if (!in_header_mode)return;

}

void FluidSolver::SensorWriter::end_header()
{
	in_header_mode = false;
}

void FluidSolver::SensorWriter::push_back(float value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::push_back(const std::string& value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::push_back(const glm::vec2& value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::push_back(const glm::vec3& value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::push_back(const glm::vec4& value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::push_back(int value)
{
	FLUID_ASSERT(!in_header_mode);
}

void FluidSolver::SensorWriter::next()
{
	FLUID_ASSERT(!in_header_mode);
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(float value)
{
	push_back(value);
	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(const std::string& value)
{
	push_back(value);
	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(const glm::vec2& value)
{
	push_back(value);
	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(const glm::vec3& value)
{
	push_back(value);
	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(const glm::vec4& value)
{
	push_back(value);
	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(Control control)
{
	switch (control) {
	case Control::Next:
		next();
		break;
		
	}

	return *this;
}

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(int value)
{
	push_back(value);
	return *this;
}
