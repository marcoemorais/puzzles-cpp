#include <cstddef>
#include <functional>
#include <iterator>
#include <unordered_set>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    // magic number
    // 0x9e3779b9
    // 32b
    // 10011110001101110111100110111001
    // no correlation between bits
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template <class Iterator>
inline std::size_t hash_range(Iterator first, Iterator last)
{
    std::size_t seed{0}; // Empty ranges hash to the same value.
    while (first != last) {
        hash_combine<typename Iterator::value_type>(seed, *first);
        ++first;
    }
    return seed;
}

template <typename Container>
struct ContainerHasher
{
    std::size_t operator()(const Container& container) const
    {
        return hash_range(std::begin(container), std::end(container));
    }
};

// Height is the height of the staircase.
typedef int Height;

// Step is the number of levels climbed on the staircase.
typedef int Step;

// Steps are sequence of steps.
typedef std::vector<Step> Steps;

// SetOfSteps is a set of steps.
typedef std::unordered_set<Steps, ContainerHasher<Steps>> SetOfSteps;

// staircase returns the set of all possible ways to climb
// a staircase of height h.
SetOfSteps
staircase(const Height h, const Steps& allowed_steps)
{
    SetOfSteps ways{};

    if (h == 0) {
        return ways;
    }

    for (const auto& s : allowed_steps) {
        if (s > h) {
            continue; // Skip steps which exceed height.
        }
        if (s == h) {
            // Handle single step.
            ways.emplace(Steps{s});
        }
        else {
            // Handle combinations of steps.
            auto s1 = staircase(h-s, allowed_steps);
            auto s2 = staircase(s, allowed_steps);
            for (const auto& s1i : s1) {
                for (const auto& s2i : s2) {
                    // Concatenate [s1, s2].
                    Steps s1s2(s1i);
                    s1s2.insert(std::end(s1s2),
                                std::begin(s2i), std::end(s2i));
                    ways.emplace(s1s2);

                    // Concatenate [s2, s1]
                    Steps s2s1(s2i);
                    s2s1.insert(std::end(s2s1),
                                std::begin(s1i), std::end(s1i));
                    ways.emplace(s2s1);
                }
            }
        }
    }

    return ways;
}

TEST_CASE("examples", "[staircase]")
{
    struct test_case
    {
        Height h;
        Steps allowed_steps;
        SetOfSteps expected_steps;
    };

    std::vector<test_case> test_cases{
        // Base case.
        {
            0,
            {1, 2, 3},
            {}
        },
        // Single step.
        {
            1,
            {1, 2, 3},
            {{1}}
        },
        // Simplest multi-step.
        {
            2,
            {1, 2, 3},
            {{1, 1}, {2}}
        },
        // Height=3.
        {
            3,
            {1, 2, 3},
            {
                {1, 1, 1},
                {1, 2}, {2, 1},
                {3}
            }
        },
        // Height=4.
        {
            4,
            {1, 2, 3},
            {
                {1, 1, 1, 1},
                {2, 1, 1}, {1, 2, 1}, {1, 1, 2},
                {2, 2},
                {1, 3}, {3, 1}
            }
        }
    };

    for (const auto& c : test_cases) {
        CAPTURE(c.h, c.allowed_steps);
        auto rcv = staircase(c.h, c.allowed_steps);
        REQUIRE(rcv == c.expected_steps);
    }
}
