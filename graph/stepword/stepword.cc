#include <algorithm>
#include <string>
#include <iterator>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// stepword_chain returns the shortest path of one-character transformations
// from start to end consisting only of valid_words.
std::vector<std::string>
stepword_chain(const std::string& start,
               const std::string& end,
               const std::vector<std::string>& valid_words)
{
    // nodes_to_visit is queue of words used to perform breadth first search
    // over the graph formed by connecting one-character transformations.
    std::queue<std::string> nodes_to_visit;
    nodes_to_visit.emplace(start);

    // parents maps a word to the word which precedes it in the sequence.
    std::unordered_map<std::string, std::string> parents;
    parents[start] = start; // By convention, start is mapped to itself.

    // visit_states records the state of each valid word.
    enum class VisitState { Undiscovered, Discovered, Visited };
    std::unordered_map<std::string, VisitState> visit_states;

    // is_one_character_different is true when s1 and s2 differ by one char.
    auto is_one_character_different =
        [](const std::string& s1, const std::string& s2) -> bool {
        if (s1.size() != s2.size()) {
            return false;
        }
        int cnt_diff = 0;
        for (std::size_t ii = 0; ii < s1.size(); ++ii) {
            if (s1[ii] != s2[ii] && ++cnt_diff > 1) {
                break;
            }
        }
        return cnt_diff == 1;
    };

    while (!nodes_to_visit.empty()) {
        auto word = nodes_to_visit.front();
        nodes_to_visit.pop();
        visit_states[word] = VisitState::Visited;

        for (const auto& next_word : valid_words) {
            // Skip words which have already been visited since in a breadth
            // first search they are part of a shorter transformation sequence.
            // NOTE(mmorais): Not exactly sure about the criteria here.
            // Should we exclude Discovered or Visited or something else?
            auto next_word_state = visit_states.find(next_word);
            if (next_word_state != visit_states.end()
                && next_word_state->second == VisitState::Visited) {
                continue;
            }

            // Skip words if more than one-character edit is required.
            if (!is_one_character_different(word, next_word)) {
                continue;
            }

            // Set the parent of the next_word to word and add next_word to
            // the list of nodes to visit to continue the sequence.
            parents[next_word] = word;
            nodes_to_visit.emplace(next_word);
        }
    }

    // Step backwards through the transformation from end back to start.
    std::vector<std::string> shortest_path;
    for (auto p = parents.find(end);
            p != std::end(parents);
            p = parents.find(p->second)) {
        shortest_path.emplace_back(p->first);
        if (p->first == start) {
            break;
        }
    }
    std::reverse(std::begin(shortest_path), std::end(shortest_path));

    return shortest_path;
}


TEST_CASE("examples", "[stepword]")
{
    struct test_case
    {
        std::string start;
        std::string end;
        std::vector<std::string> valid_words;
        std::vector<std::string> expected_path;
    };

    std::vector<test_case> test_cases{
        test_case{
            "dog",
            "cat",
            {"dog", "dot", "dop", "dat", "cat"},
            {"dog", "dot", "dat", "cat"}
        },
        test_case{
            "best",
            "rise",
            {"best", "four", "ruse", "hour", "rise", "home", "fill", "memo",
             "bust", "type", "also", "pack", "time", "look", "only", "rust"},
            {"best", "bust", "rust", "ruse", "rise"}
        },
        // Negative example.
        test_case{
            "dog",
            "cat",
            {"dog", "dot", "tod", "mat", "cat"},
            {}
        }
    };

    for (const auto& c : test_cases) {
        CAPTURE(c.start, c.end, c.valid_words, c.expected_path);
        auto rcv_path = stepword_chain(c.start, c.end, c.valid_words);

        REQUIRE(rcv_path == c.expected_path);
    }
}
