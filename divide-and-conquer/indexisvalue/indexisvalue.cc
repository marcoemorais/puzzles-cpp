#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// index_matches_value returns the range [start, end) for which A[i] = i.
template <typename IntegerT,
          std::enable_if_t<std::is_integral<IntegerT>::value>* = nullptr>
std::optional<std::pair<IntegerT, IntegerT>>
index_matches_value(
    const std::vector<IntegerT>& A,
    const IntegerT first,
    const IntegerT last)
{
    if (first == last) {
        return {};
    }

    auto mid = first + (last - first) / 2; // Avoid overflow.

    // If value less than index, then check right subarray.
    if (A[mid] < mid) {
        return index_matches_value(A, mid+1, last);
    }
    // If value greater than index, then check left subarray.
    else if (A[mid] > mid) {
        return index_matches_value(A, first, mid);
    }

    // Found a value matching index. Find [start, end) of run.
    auto start = mid, end = mid;

    // Scan left until A[start] != start.
    while (start != first) {
        --start;
        if (A[start] != start) {
            ++start;
            break;
        }
    }

    // Scan right until A[end] != end.
    while (end != last) {
        ++end;
        if (A[end] != end) {
            break;
        }
    }

    return std::make_pair(start, end);
}

TEST_CASE("examples", "[indexisvalue]")
{
    using T = int;

    struct test_case
    {
        std::vector<T> input;
        std::optional<std::pair<T, T>> expected_rcv;
    };

    std::vector<test_case> test_cases{
        // Edge case, empty array.
        {
            {},
            {}
        },
        // Edge case, one element array.
        {
            {0},
            std::make_pair(0, 1)
        },
        // Run of matching indices in middle.
        {
            {-10, -5, 0, 3, 4, 5, 7, 20},
            std::make_pair(3, 6)
        },
        // Run of matching indices in left subarray.
        {
            {0, 1, 2, 5, 7, 8, 9, 10},
            std::make_pair(0, 3)
        },
        // Run of matching indices in right subarray.
        {
            {-10, -5, 0, 1, 4, 5, 10},
            std::make_pair(4, 6)
        }
    };

    for (const auto& c : test_cases) {
        CAPTURE(c.input);
        auto rcv = index_matches_value(c.input, T(0), T(c.input.size()));
        REQUIRE(bool(rcv) == bool(c.expected_rcv));
        if (c.expected_rcv) {
            REQUIRE((*rcv).first == (*c.expected_rcv).first);
            REQUIRE((*rcv).second == (*c.expected_rcv).second);
        }
    }
}
