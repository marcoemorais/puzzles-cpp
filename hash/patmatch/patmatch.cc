#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>
#include <optional>
#include <random>
#include <type_traits>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// fastpow uses divide and conquer to compute integer power.
template <typename T, typename IntegerT,
          std::enable_if_t<std::is_integral<IntegerT>::value>* = nullptr>
T fastpow(const T& y, const IntegerT& x)
{
    // Base case.
    if (x == IntegerT{0}) {
        return T{1};
    }

    // Compute $y^x = y^{\floor{x/2}} \times y^{\ceil{x/2}}$.
    IntegerT absx = x < IntegerT{0} ? -x : x;
    IntegerT floorx = absx/IntegerT{2}; // Round toward zero.
    T yhalfx = fastpow(y, floorx);
    // If x is even, then return yx, else multiply by additional term.
    T yx = absx%2 == 0 ? yhalfx*yhalfx : yhalfx*yhalfx*y;

    // For negative exponents, return 1/y^x.
    return x < IntegerT{0} ? T{1}/yx : yx;
}

// is_bidirectional_iterator returns true when iterator is bidirectional.
template<typename iterator>
using is_bidirectional_iterator =
	std::disjunction<
  		std::is_same<
			typename std::iterator_traits<iterator>::iterator_category,
			std::bidirectional_iterator_tag>,
		// Without this disjunction, random access iterators fail to qualify.
  		std::is_same<
			typename std::iterator_traits<iterator>::iterator_category,
			std::random_access_iterator_tag>
	>;

// rolling_hash computes hash of [first, last) in O(1) using prior hash value.
template <typename Iter,
		  typename HashT=std::uint64_t,
		  std::enable_if_t<is_bidirectional_iterator<Iter>::value>* = nullptr>
HashT
rolling_hash(Iter first, Iter last, const HashT& previous=0)
{
    static constexpr HashT base{131}; // Next prime after 2^7.

    HashT hash{0U};
    if (previous) {
        // Compute a new hash from the previous by subtracting the value
        // immediately preceeding first from the new hash and adding the
        // value immediately preceeding the last to the new hash.
        hash = previous;
        auto len = std::distance(first, last);
		--first;
        hash -= *first * fastpow(base, len-1);
		--last;
		hash = (hash*base) + *last;
    }
    else {
        // Compute a new hash by traversing from [first, last).
        while (first != last) {
            hash = (hash*base) + *first++;
        }
    }

    return hash;
}

// patmatch finds the first occurrence of a pattern in sequence [first, last).
template <typename Iter,
		  std::enable_if_t<is_bidirectional_iterator<Iter>::value>* = nullptr>
std::optional<Iter>
patmatch(Iter first, Iter last, Iter pat_beg, Iter pat_end)
{
    auto patlen = std::distance(pat_beg, pat_end);
    auto seqlen = std::distance(first, last);
    if (patlen > seqlen || seqlen < 1) {
        return {};
    }

	auto pathash = rolling_hash(pat_beg, pat_end);

	// Compute the hash using a window of size patlen.
	auto end = first + patlen;
	auto seqhash = rolling_hash(first, end);

	// Handle edge case of match at start of sequence.
	if (pathash == seqhash && std::equal(first, end, pat_beg, pat_end)) {
		return first; // Return iterator to start of pattern.
	}
	++first, ++end;

    auto search_end = last - (patlen-1);
    while (first != search_end) {
		// Update the rolling hash with the next value in the sequence.
		seqhash = rolling_hash(first, end, seqhash);

		// If the hashes match, avoid false positive with full comparison.
		if (pathash == seqhash && std::equal(first, end, pat_beg, pat_end)) {
			return first; // Return iterator to start of pattern.
		}

		// Advance the window.
        ++first, ++end;
    }

    return {}; // No pattern found.
}

TEST_CASE("examples", "[patmatch]")
{
    using T = std::uint8_t;

    struct test_case
    {
        std::vector<T> input;
        std::vector<T> pattern;
        std::optional<std::size_t> expected;
    };

    std::vector<test_case> test_cases{
        // Pattern not found.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {2,4,6,8},
            {}
        },
        // Pattern at start of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {0,1,2,3},
            {0}
        },
        // Pattern at middle of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {3,4,5,6},
            {3}
        },
        // Pattern at end of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {6,7,8,9},
            {6}
        },
        // Pattern partial match at start of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {0,1,9},
            {}
        },
        // Pattern partial match at middle of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {3,4,0},
            {}
        },
        // Pattern partial match at end of input.
        {
            {0,1,2,3,4,5,6,7,8,9},
            {7,8,0},
            {}
        }
    };

    for (const auto& c : test_cases) {
        auto rcv = patmatch(std::begin(c.input), std::end(c.input),
                            std::begin(c.pattern), std::end(c.pattern));
        CAPTURE(c.input, c.pattern, c.expected);
        if (bool(c.expected)) {
            REQUIRE(bool(rcv) == true);
            // Convert iterator to index.
            auto ind = std::distance(std::begin(c.input), *rcv);
            REQUIRE(ind == *c.expected);
        } else {
            REQUIRE(bool(rcv) == false);
        }
    }
}

TEST_CASE("random", "[patmatch]")
{
    using T = std::uint32_t;

	// Use case is long sequence and range of pattern lengths.
	std::size_t sequencelen{10000};
	//std::size_t sequencelen{10};
	std::vector<std::size_t> patlens{32,64,128,256,512,1024};
	//std::vector<std::size_t> patlens{5};

    // Random sample parameters.
    std::mt19937 gen{std::random_device{}()};
	std::uniform_int_distribution<> dis(0, sequencelen-1);

	std::size_t repeat{100};

	// Initialize monotonic sequence from [1,sequencelen].
	std::vector<T> sequence(sequencelen);
	std::iota(std::begin(sequence), std::end(sequence), T{1});

	// Reserve buffer for longest pattern.
	std::vector<T> pattern(patlens.back());

	// Repeat the following steps for each pattern length tested.
	// 1. Shuffle the sequence.
	// 2. Select a random index from within the sequence.
	// 3. Copy the elements from the sequence to the pattern.
	// 4. Confirm the returned index matches the index from step 2.
	// 5. Shuffle the pattern.
	// 6. As a negative test, confirm the pattern does not match sequence.
	do {
		for (const auto& patlen : patlens) {
			CAPTURE(repeat, sequencelen, patlen);

			// Setup sequence and pattern.
			std::shuffle(std::begin(sequence), std::end(sequence), gen);
			auto ind = dis(gen);
			if (ind+patlen > sequencelen) {
				ind -= (sequencelen-patlen);
			}
			std::copy(std::begin(sequence)+ind,
					  std::begin(sequence)+ind+patlen,
					  std::begin(pattern));

			// Positive test.
			{
				auto rcv = patmatch(std::begin(sequence), std::end(sequence),
									std::begin(pattern),
									std::begin(pattern)+patlen);
				REQUIRE(bool(rcv) == true);
				// Convert iterator to index.
				auto rcv_ind = std::distance(std::begin(sequence), *rcv);
				REQUIRE(rcv_ind == ind);
			}

			// Negative test.
			{
				std::shuffle(std::begin(pattern), std::begin(pattern)+patlen,
							 gen);
				auto rcv = patmatch(std::begin(sequence), std::end(sequence),
									std::begin(pattern),
									std::begin(pattern)+patlen);
				REQUIRE(bool(rcv) == false);
			}
		}

		--repeat;
	} while(repeat > std::size_t{0});
}
