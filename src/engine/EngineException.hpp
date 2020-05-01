#ifndef ENGINE_ENGINEEXCEPTION_HPP
#define ENGINE_ENGINEEXCEPTION_HPP


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
#endif //ENGINE_ENGINEEXCEPTION_HPP
