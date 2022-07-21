#include "CompatibilityReport.hpp"

#include "Log.hpp"

#include <fmt/core.h>

namespace LibFluid {

    void CompatibilityReport::log_issues() const {
        for (auto& issue : issues) {
            Log::error(fmt::format("[Compatibility Issue] {}: {}", issue.scope_as_string(), issue.issue_message));
        }
    }

    bool CompatibilityReport::has_issues() const {
        return !issues.empty();
    }

    void CompatibilityReport::add_issue(std::string issue_message) {
        issues.push_back({scope, issue_message});
    }

    void CompatibilityReport::begin_scope(std::string scope_name) {
        scope.push_back(scope_name);
    }

    void CompatibilityReport::end_scope() {
        scope.pop_back();
    }

    std::string CompatibilityReport::CompatibilityIssue::scope_as_string() const {
        std::string res = "";
        for (auto& s : scope) {
            if (!res.empty()) {
                res += " -> ";
            }
            res += s;
        }
        return res;
    }
} // namespace FluidSolver