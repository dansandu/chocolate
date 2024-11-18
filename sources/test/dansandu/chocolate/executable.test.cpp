#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/utility.hpp"

#include <string>

using dansandu::radiance::progress_bar_console_reporter::ProgressBarConsoleReporter;
using dansandu::radiance::test_case_registry::TestCaseRegistry;
using dansandu::radiance::utility::getEnvironmentVariable;
using dansandu::radiance::utility::toWideString;

int main(const int argc, const char* const* const argv)
{
    const auto stageIndexString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_INDEX");
    const auto stageIndex = stageIndexString.has_value() ? std::stoi(stageIndexString.value()) : 0;

    const auto stageCountString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_COUNT");
    const auto stageCount = stageCountString.has_value() ? std::stoi(stageCountString.value()) : 0;

    auto reporter = ProgressBarConsoleReporter{stageIndex, stageCount};

    if (argc > 1)
    {
        const auto testCaseName = toWideString(argv[1]);

        const auto testSuiteResult = TestCaseRegistry::instance().runTestCase(testCaseName, reporter);

        return !testSuiteResult.testSuiteSuccess;
    }
    else
    {
        const auto testSuiteResult = TestCaseRegistry::instance().runAllTestCases(reporter);

        return !testSuiteResult.testSuiteSuccess;
    }
}
