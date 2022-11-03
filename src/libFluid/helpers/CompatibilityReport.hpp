#pragma once

#include <string>
#include <vector>

namespace LibFluid {



    class CompatibilityReport {

      public:

        void add_issue(std::string issue_message);

        void begin_scope(std::string scope_name);

        void end_scope();

        bool has_issues() const;

        void log_issues() const;

      private:
        struct CompatibilityIssue
        {
            std::vector<std::string> scope;
            std::string issue_message;

            std::string scope_as_string() const;
        };


        std::vector<CompatibilityIssue> issues;

        std::vector<std::string> scope;
    };


} // namespace FluidSolver
