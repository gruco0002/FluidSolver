#include "SolverSerializer.hpp"
namespace LibFluid::Serialization {

    nlohmann::json SolverSerializer::serialize(std::shared_ptr<IFluidSolverBase> solver) {
        return nlohmann::json();
    }
}