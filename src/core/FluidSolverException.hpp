//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVEREXCEPTION_HPP
#define FLUIDSOLVER_FLUIDSOLVEREXCEPTION_HPP

#include <exception>
#include <string>

namespace FluidSolver {
class FluidSolverException : public std::exception {
public:

    std::string text;

    explicit FluidSolverException(std::string text);

    const char *what() const noexcept override;
};
}


#endif //FLUIDSOLVER_FLUIDSOLVEREXCEPTION_HPP
