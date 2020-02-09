#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <tuple>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// BinaryIndexTree provides logarithmic add and cumsum operations without any
// additional storage overhead.
template <typename T,
          std::enable_if_t<std::is_integral<T>::value>* = nullptr>
class BinaryIndexedTree
{
  public:

    // BinaryIndexedTree holds n samples from [1, n].
    BinaryIndexedTree(const std::size_t n)
        : tree_(n+1)
    { }

    // add increments the bin at t by x.
    void add(std::size_t t, T x)
    {
        assert(t > 0); // t must start from 1.

        // Increment the value in all bins from t until end of tree
        // by increasing t to the next power of 2 at each iteration.
        //
        // For example, assume t = 3d.
        // Increment bin at index 3 by x.
        //
        // Increase t by the next power of 2 from 3d to 4d.
        // Increment bin at index 4 by x.
        //
        // Increase t by the next power of 2 from 4d to 8d.
        // Increment bin at index 8 by x.
        //
        // Stop when index is larger than largest bin.
        std::size_t ind{t};
        while (ind < tree_.size()) {
            tree_[ind] += x;
            ind += ind & (-ind); // Right parent one-level higher in tree.
        }
    }

    // cumsum returns cumulative sum of bins from [t0, t1].
    T cumsum(std::size_t t0, std::size_t t1)
    {
        assert(t0 > 0); // t0 must start from 1.
        assert(t1 >= t0);

        // If range does not start at 1, then treat as difference of ranges.
        if (t0 != 1) {
            return cumsum(1, t1) - cumsum(1, t0-1);
        }

        // Accumulate the sum of values from bins whose indices are
        // computed by subtracting the lowest nonzero power of 2.
        //
        // For example, assume t1 = 9d.
        // Start with value at index 9d (9d = 2^3 + 2^0 = 101).
        //
        // Subtract lowest nonzero power of 2 from 9d yields 8d.
        // Add the value at index 8d (8d = 2^3 = 100).
        //
        // Subtract lowest nonzero power of 2 from 8d yields 0d.
        // Index 0d is not a valid bin and ends the traversal.
        T sum{0};
        std::size_t ind{t1};
        while (ind > 0) {
            sum += tree_[ind];
            ind -= ind & (-ind); // Left parent one-level higher in tree.
        }

        return sum;
    }

  private:
    // tree_ holds the binary indexed tree as a flat array.
    std::vector<T> tree_;
};

TEST_CASE("examples", "[BinaryIndexedTree]")
{
    using T = std::int64_t;

    std::size_t n{14};
    BinaryIndexedTree<T> bit{n};

    // Initialize the tree with pairs of (t, x).
    typedef std::tuple<std::size_t, T> Op;
    std::vector<Op> ops{
        {1,1},{2,7},{3,3},{4,0},{5,5},{6,8},{7,3},
        {8,2},{9,6},{10,2},{11,1},{12,1},{13,4},{14,5}
    };
    for (const auto& op : ops) {
        auto t = std::get<0>(op);
        auto x = std::get<1>(op);
        bit.add(t, x);
    }

    // Compare expected sum starting from 1 at each value of t.
    typedef std::tuple<std::size_t, std::size_t, T> Sum;
    std::vector<Sum> sums{
        {1,1,1}, {1,2,8}, {1,3,11}, {1,4,11}, {1,5,16}, {1,6,24}, {1,7,27},
        {1,8,29}, {1,9,35}, {1,10,37}, {1,11,38}, {1,12,39}, {1,13,43},
        {1,14,48}
    };
    for (const auto& sum : sums) {
        auto t0 = std::get<0>(sum);
        auto t1 = std::get<1>(sum);
        auto expected = std::get<2>(sum);
        auto rcv = bit.cumsum(t0, t1);
        CAPTURE(t0, t1, expected, rcv);
        REQUIRE(rcv == expected);
    }

    // Compare some ranges not starting from 1.
    std::vector<Sum> sums2{
        {5,7,16},{3,4,3},{2,14,47},{9,9,6}
    };
    for (const auto& sum : sums2) {
        auto t0 = std::get<0>(sum);
        auto t1 = std::get<1>(sum);
        auto expected = std::get<2>(sum);
        auto rcv = bit.cumsum(t0, t1);
        CAPTURE(t0, t1, expected, rcv);
        REQUIRE(rcv == expected);
    }

    // Add 1 to each bin and verify that sums are updated.
    for (std::size_t i = 0; i < sums.size(); ++i) {
        auto t1 = std::get<1>(sums[i]);
        bit.add(t1, 1);
        // Increment the expected sum by 1 for all bins >= i.
        for (std::size_t j = i; j < sums.size(); ++j) {
            std::get<2>(sums[j]) += 1;
        }
        // Compare expected sum across all bins in tree.
        for (const auto& sum : sums) {
            auto t0 = std::get<0>(sum);
            auto t1 = std::get<1>(sum);
            auto expected = std::get<2>(sum);
            auto rcv = bit.cumsum(t0, t1);
            CAPTURE(t0, t1, expected, rcv);
            REQUIRE(rcv == expected);
        }
    }
}
