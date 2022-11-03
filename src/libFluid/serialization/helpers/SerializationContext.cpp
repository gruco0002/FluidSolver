#include "SerializationContext.hpp"
namespace LibFluid::Serialization {

    void SerializationContext::begin_section(const std::string& name) {
        section_stack.push_back(name);
    }

    void SerializationContext::end_section() {
        if (!section_stack.empty()) {
            section_stack.pop_back();
        }
    }

    void SerializationContext::add_issue(const std::string& message) {
        Issue issue;
        issue.section_stack = section_stack;
        issue.message = message;

        issues.push_back(std::move(issue));
    }

    std::string SerializationContext::Issue::to_formatted_string() const {
        std::string result;

        bool first = true;
        for (const auto& section : section_stack) {
            if (!first) {
                result.append(".");
            }
            first = false;
            result.append(section);
        }

        if (!result.empty()) {
            result.append(": ");
        }

        result.append(message);

        return result;
    }
} // namespace LibFluid::Serialization