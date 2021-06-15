#include "Compatibility.hpp"

#include "Log.hpp"

#include <fmt/core.h>

namespace FluidSolver
{

    void Compatibility::add_issue(const CompatibilityIssue& problem)
    {
        issues.push_back(problem);
    }

    void Compatibility::log_issues() const
    {
        for (auto& issue : issues)
        {
            Log::error(fmt::format("[Compatibility Issue] {}: {}", issue.component_name, issue.issue_message));
        }
    }

    bool Compatibility::has_issues() const
    {
        return !issues.empty();
    }

    void Compatibility::add_compatibility(const Compatibility& other)
    {
        for (auto& issue : other.issues)
        {
            issues.push_back(issue);
        }
    }


} // namespace FluidSolver