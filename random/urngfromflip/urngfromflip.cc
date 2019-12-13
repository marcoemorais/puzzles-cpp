#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <random>
#include <type_traits>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// flip returns 0 or 1 with equal probability.
std::uint8_t
flip()
{
    thread_local std::mt19937 gen{std::random_device{}()};
    constexpr double a = 0., b = 1., bias = 0.5;
    std::uniform_int_distribution<> dis(a, b);
    // NOTE(mmorais): Alter the value of bias to observe in change in chisq.
    return dis(gen) < bias ? 0 : 1;
}

// urngfromflip returns a random integer in the range [a, b].
template <typename IntegerT,
          std::enable_if_t<std::is_integral<IntegerT>::value>* = nullptr>
IntegerT
urngfromflip(const IntegerT& a, const IntegerT& b)
{
    using T = std::uint64_t;
    // Compute the range.
    T n = b-a;
    // Compute the number of bits to represent the range.
    T nbits = static_cast<T>(std::ceil(std::log(n)/std::log(2)));
    // Use flip to obtain a value for each bit in nbits.
    std::uint64_t urng;
    do {
        urng = 0;
        // Concatenate each bit from least significant to most significant.
        for (T pos = 0; pos < nbits; ++pos) {
            urng = urng | (flip()<<pos);
        }
    } while (urng > n);
    // Add the random integer back to the lower bound.
    return a + static_cast<IntegerT>(urng);
}

// chisq bins integers x and computes the chisq statistic based on a, b.
template <typename T>
double
chisq(const std::vector<T>& x, T a, T b)
{
    // Compute the number of bins from the range of the distribution.
    T n = b-a+T{1};
    // Compute the histogram of x.
    std::vector<std::uint64_t> histx(n, 0);
    for (const auto& xi : x) {
        // Increment the count at this bin.
        ++histx[xi-a];
    }
    // Compute the expected count in each bin.
    double expected = static_cast<double>(x.size())/n;
    // Compute the sum of squares of observed minus expected over expected.
    double chisq = std::accumulate(std::begin(histx), std::end(histx), 0.0,
        [&expected](double ssq, std::uint64_t xi) {
            return ssq + (xi-expected)*(xi-expected)/expected;
        }
    );
    return chisq;
}

TEST_CASE("examples", "[urngfromflip]")
{
    using T = std::uint32_t;

    struct test_case
    {
        T a;
        T b;
    };

    std::vector<test_case> test_cases{
        { 0, 255 },
        { 100, 110 }
    };

    // Test parameters.
    std::size_t num_tests{100};

    for (const auto& c : test_cases) {
        auto repeat = num_tests;
        do {
            auto rcv = urngfromflip(c.a, c.b);
            CAPTURE(c.a, c.b, rcv);
            bool inrange = rcv >= c.a && rcv <= c.b;
            REQUIRE(inrange == true);
            --repeat;
        } while (repeat > std::size_t{0});
    }
}

TEST_CASE("chisq", "[urngfromflip]")
{
    using T = std::uint8_t;

    // Simulation parameters.
    T a = 1, b = 100;
    std::size_t nsamples = 1000;

    // Collect random samples.
    std::vector<T> randx(nsamples, T{0});
    for (std::size_t i = 0; i < nsamples; ++i) {
        randx[i] = urngfromflip(a, b);
    }

    // Compute the chisq statistic and compare to pvalue.
    double chisqx = chisq(randx, a, b);
    int df = b-a; // 99
    constexpr double pvalue = 148.21; // P=0.001
    CAPTURE(a, b, nsamples, df, chisqx, pvalue);
    // Null hypothesis H0 is that numbers are from uniform distribution.
    // When chisqx <= pvalue, then we fail to reject the null hypothesis.
    REQUIRE(chisqx <= pvalue);
}
