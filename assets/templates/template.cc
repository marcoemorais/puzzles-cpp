#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

TEST_CASE("examples", "[template]")
{
    struct test_case
    {
    };

    std::vector<test_case> test_cases{
    };

    for (const auto& c : test_cases) {
    }
}
