//
// Created by corbi on 16.04.2019.
//

#ifndef FLUIDSOLVER_ENGINEEXCEPTION_HPP
#define FLUIDSOLVER_ENGINEEXCEPTION_HPP


#include <exception>

namespace Engine {
    class EngineException : public std::exception {

    public:
        explicit EngineException(char const *const _Message);

        const char *what() const override;
    };

}
#endif //FLUIDSOLVER_ENGINEEXCEPTION_HPP
