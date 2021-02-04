#include "SensorWriter.hpp"

void FluidSolver::SensorWriter::push_back_header(const std::string& header, size_t dimensionality)
{
	if (!in_header_mode)return;
	FLUID_ASSERT(dimensionality > 0);

	if (header_count > 0) stream << SEPERATOR;

	dimensionality_info.push_back(dimensionality);
	if (dimensionality == 1) {
		stream << header;
	}
	else {
		stream << header << " - 0";
		for (size_t i = 1; i < dimensionality; i++) {
			dimensionality_info.push_back(dimensionality);
			stream << SEPERATOR << header << " - " << i;
		}
	}

	header_count += dimensionality;
}

void FluidSolver::SensorWriter::end_header()
{
	if (!in_header_mode) return;
	in_header_mode = false;

	stream << NEWLINE;
}

void FluidSolver::SensorWriter::next_value()
{
	FLUID_ASSERT(current__row_position < header_count);
	FLUID_ASSERT(!in_header_mode);
	current__row_position++;
}

void FluidSolver::SensorWriter::add_seperator_if_required()
{
	FLUID_ASSERT(!in_header_mode);
	if (current__row_position > 0)
		stream << SEPERATOR;
}

void FluidSolver::SensorWriter::check_correct_dimensionality_at_position(size_t dimensionality) const
{
	FLUID_ASSERT(!in_header_mode);
	FLUID_ASSERT(current__row_position < dimensionality_info.size());

	FLUID_ASSERT(dimensionality == dimensionality_info[current__row_position]);
}

FluidSolver::SensorWriter::SensorWriter(const std::string& filepath) : filepath(filepath), stream(filepath, std::ios::out) {

}

void FluidSolver::SensorWriter::push_back(float value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(1);

	add_seperator_if_required();
	stream << value;
	next_value();
}

void FluidSolver::SensorWriter::push_back(const std::string& value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(1);

	add_seperator_if_required();
	stream << value;
	next_value();

}

void FluidSolver::SensorWriter::push_back(const glm::vec2& value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(2);

	add_seperator_if_required();
	stream << value.x;
	next_value();

	add_seperator_if_required();
	stream << value.y;
	next_value();

}

void FluidSolver::SensorWriter::push_back(const glm::vec3& value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(3);

	add_seperator_if_required();
	stream << value.x;
	next_value();

	add_seperator_if_required();
	stream << value.y;
	next_value();

	add_seperator_if_required();
	stream << value.z;
	next_value();

}

void FluidSolver::SensorWriter::push_back(const glm::vec4& value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(4);

	add_seperator_if_required();
	stream << value.x;
	next_value();

	add_seperator_if_required();
	stream << value.y;
	next_value();

	add_seperator_if_required();
	stream << value.z;
	next_value();

	add_seperator_if_required();
	stream << value.w;
	next_value();
}

void FluidSolver::SensorWriter::push_back(int value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(1);

	add_seperator_if_required();
	stream << value;
	next_value();
}

void FluidSolver::SensorWriter::push_back(const FluidSolver::Timepoint& value)
{
	FLUID_ASSERT(!in_header_mode);

	check_correct_dimensionality_at_position(3);

	add_seperator_if_required();
	stream << value.timestep_number;
	next_value();

	add_seperator_if_required();
	stream << value.simulation_time;
	next_value();

	/*add_seperator_if_required();
	stream << value.system_time;
	next_value();*/

	add_seperator_if_required();
	stream << value.current_time_step;
	next_value();

}

void FluidSolver::SensorWriter::next()
{
	FLUID_ASSERT(!in_header_mode);

	if (current__row_position == 0) return;

	while (current__row_position < header_count) {
		add_seperator_if_required();
		next_value();
	}
	stream << NEWLINE;
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

FluidSolver::SensorWriter& FluidSolver::SensorWriter::operator<<(const FluidSolver::Timepoint& value)
{
	push_back(value);
	return *this;
}




