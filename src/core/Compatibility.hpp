#pragma once

#include <string>
#include <vector>

namespace FluidSolver
{

    struct CompatibilityIssue
    {
        std::string component_name;
        std::string issue_message;
    };

    class Compatibility {

      public:
        void add_issue(const CompatibilityIssue& problem);
        bool has_issues() const;

        void log_issues() const;

        void add_compatibility(const Compatibility& other);

      private:
        std::vector<CompatibilityIssue> issues;
    };

} // namespace FluidSolver
