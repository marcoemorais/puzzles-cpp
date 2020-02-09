#include <algorithm>
#include <cstddef>
#include <iostream>
#include <istream>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// randstream returns random sample of k elements from an unbounded stream.
template <typename T>
std::vector<T>
randstream(std::istream& is, std::size_t k=1)
{
    thread_local std::mt19937 gen{std::random_device{}()};

    std::vector<T> samples;
    samples.reserve(k);
    T x;

    // Fill the reservoir with the first k samples.
    while (is >> x) {
        samples.emplace_back(x);
        if (samples.size() == k) {
            break;
        }
    }

    // For each sample received after the reservoir is filled,
    // replace an existing sample with probability = 1/n.
    std::size_t n{k};
    while (is >> x) {
        ++n;
        std::uniform_int_distribution<std::size_t> dis(1, n); // [1, n]
        auto rind = dis(gen);
        if (rind <= k) {
            samples[rind-1] = x;
        }
    }

    return samples;
}

TEST_CASE("chisq", "[randstream]")
{
    using T = std::uint32_t;

    // Simulation parameters.
    std::size_t k = 10; // Number of samples to return from each trial.
    std::size_t n = 100; // Size of the stream.
    std::size_t nrepeat = 10000; // Number of trials.

    // Compute the expected count in each bin.
    double expected = static_cast<double>(nrepeat)*
        static_cast<double>(k)/static_cast<double>(n);

    // Random number generator used to shuffle the input stream.
    std::mt19937 gen{std::random_device{}()};

    // Initialize monotonic sequence from [1, n].
    std::vector<T> sequence(n);
    std::iota(std::begin(sequence), std::end(sequence), T{1});

    // Initialize a histogram to record frequency of each sample.
    std::vector<std::uint32_t> hist(n, 0);

    do {
        // Shuffle the sequence.
        std::shuffle(std::begin(sequence), std::end(sequence), gen);

        // Copy the sequence into a character-based stream.
        std::stringstream ss;
        std::copy(std::begin(sequence), std::end(sequence),
                  std::ostream_iterator<T>(ss, " "));

        // Obtain k random samples from stream of size n, with k < n.
        auto rsamples = randstream<T>(ss, k);
        REQUIRE(rsamples.size() == k);

        // Increment the count for each sample.
        for (const auto& x : rsamples) {
            hist[x-1] += 1;
        }

        --nrepeat;
    } while (nrepeat > std::size_t{0});

    // Print the histogram bins.
    for (std::size_t ii = 0; ii < hist.size(); ++ii) {
        CAPTURE(ii, hist[ii]);
    }

    // Compute the sum of squares of observed minus expected over expected.
    double chisq = std::accumulate(std::begin(hist), std::end(hist), 0.0,
        [&expected](double ssq, std::uint64_t xi) {
            return ssq + (xi-expected)*(xi-expected)/expected;
        }
    );

    // Compare the chisq statistic to pvalue.
    int df = n-1;
    constexpr double pvalue = 148.21; // P=0.001
    CAPTURE(k, n, expected, df, chisq, pvalue);
    // Null hypothesis H0 is that numbers are from uniform distribution.
    // When chisqx <= pvalue, then we fail to reject the null hypothesis.
    REQUIRE(chisq <= pvalue);
}
