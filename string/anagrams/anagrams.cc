#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// anagrams returns vector of indices of start of anagrams of s in word.
std::vector<std::size_t>
anagrams(const std::string& word, const std::string s)
{
    // Return a vector of start of anagrams of s in word.
    std::vector<std::size_t> matches;
    if (s.size() > word.size()) {
        return matches;  // Edge case.
    }

    // Use a character frequency map to record characters from word
    // which appear in a sliding window of length equal to size of s.
    // The map has the following invariants:
    // 0  = character appears in window and s
    // >0 = abundance of characters from s in window
    // <0 = character not in s appears in window
    std::unordered_map<std::string::value_type, int> wfreq;

    // Initialize the map with characters from s.
    for (const auto c : s) {
        auto pfreq = wfreq.find(c);
        if (pfreq != std::end(wfreq)) {
            pfreq->second += 1;
        }
        else {
            wfreq[c] = 1;
        }
    }

    // Update the map with characters from the first window in word.
    // Given the invariants described above, an anagram will only have
    // 0-valued entries in the map.  Rather than checking whether all
    // entries are 0-valued, we remove 0-valued entries from the map so
    // that the anagram test reduces to checking whether map is empty.
    for (std::size_t i = 0; i < s.size(); ++i) {
        auto pfreq = wfreq.find(word[i]);
        if (pfreq != std::end(wfreq)) {
            pfreq->second -= 1;  // Decrement means found in window.
            if (pfreq->second == 0) {
                wfreq.erase(pfreq);
            }
        }
        else {
            wfreq[word[i]] = -1;
        }
    }
    if (wfreq.size() == 0) {
        matches.emplace_back(0);
    }

    // Iterate from [1, word.size()-s.size()+1].
    for (std::size_t i = 1; i < word.size()-s.size()+1; ++i) {
        // Update the count for the character just left of window.
        const auto cleft = word[i-1];
        auto pfreq = wfreq.find(cleft);
        if (pfreq != std::end(wfreq)) {
            pfreq->second += 1;  // Increment means not found in window.
            if (pfreq->second == 0) {
                wfreq.erase(pfreq);
            }
        }
        else {
            wfreq[cleft] = 1;
        }
        // Update the count for the rightmost chracter in window.
        const auto cright = word[i+s.size()-1];
        pfreq = wfreq.find(cright);
        if (pfreq != std::end(wfreq)) {
            pfreq->second -= 1;  // Decrement means found in window.
            if (pfreq->second == 0) {
                wfreq.erase(pfreq);
            }
        }
        else {
            wfreq[cright] = -1;
        }
        // Check whether the window holds an anagram.
        if (wfreq.size() == 0) {
            matches.emplace_back(i);
        }
    }

    return matches;
}

TEST_CASE("examples", "[anagrams]")
{
    struct test_case
    {
        std::string word;
        std::string s;
        std::vector<std::size_t> expected_indices;
    };

    std::vector<test_case> test_cases{
        // One anagram at start of word followed by 2 overlapping anagrams.
        {
            "abxaba", "ab", {0, 3, 4}
        },
        // Corner case with repeating elements of s.
        {
            "abxabb", "ab", {0, 3}
        },
        // Corner case with repeating elements followed by anagram.
        {
            "abxabba", "ab", {0, 3, 5}
        },
        // Corner case with back-to-back-to-back.
        {
            "liveevilvile", "live", {0, 4, 8}
        },
        // Corner case with first match
        {
            "xbcaxacbbca", "abc", {1, 5, 8}
        },
        // Corner case with same characters but different frequency.
        {
            "ttwwt", "wwt", {1, 2}
        }
    };

    for (const auto& c : test_cases) {
        CAPTURE(c.word, c.s);
        auto rcv = anagrams(c.word, c.s);
        REQUIRE(rcv == c.expected_indices);
    }
}
