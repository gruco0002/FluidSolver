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
} // namespace LibFluid::Serialization