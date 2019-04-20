//
// Created by corbi on 16.04.2019.
//

#ifndef FLUIDSOLVER_ENGINEEXCEPTION_HPP
#define FLUIDSOLVER_ENGINEEXCEPTION_HPP


#include <exception>
#include <string>

namespace Engine {
    class EngineException : public std::exception {

    public:

        EngineException(std::string msg);

        std::string msg;

        const char *what() const noexcept override;
    };

}
#endif //FLUIDSOLVER_ENGINEEXCEPTION_HPP
