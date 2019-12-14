#include <cstddef>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// MovieID identifies a movie.
typedef std::string MovieID;

// UserID identifies a user.
typedef std::string UserID;

// MovieTable is a table with movies attended by each user.
typedef std::vector<std::tuple<MovieID, UserID>> MovieTable;

// Score is the similarity score for a pair of movies.
typedef std::tuple<MovieID, MovieID, double> Score;

// GreaterThanScore orders Score by the score.
struct GreaterThanScore
{
    bool operator()(const Score& lhs, const Score& rhs)
    {
        return std::get<2>(lhs) > std::get<2>(rhs);
    }
};

// UniqueUsers is a set of unique UserID.
typedef std::set<UserID> UniqueUsers;

// similarity returns the similarity between the set of users.
double similarity(const UniqueUsers& u1, const UniqueUsers& u2)
{
    std::set<UserID> uu_int;
    std::set_intersection(std::begin(u1), std::end(u1),
                          std::begin(u2), std::end(u2),
                          std::inserter(uu_int, std::begin(uu_int)));

    std::set<UserID> uu_union;
    std::set_union(std::begin(u1), std::end(u1),
                   std::begin(u2), std::end(u2),
                   std::inserter(uu_union, std::begin(uu_union)));

    return 1.0 * uu_int.size() / uu_union.size();
}

// topk_similar_movies returns k-most-similar movies based on common users.
std::vector<Score>
topk_similar_movies(const MovieTable& movies, const std::size_t k)
{
    // Map each movie to the set of users who attended.
    std::unordered_map<MovieID, UniqueUsers> users;
    for (const auto& m : movies) {
        auto u = users.find(std::get<0>(m));
        if (u == users.end()) {
            auto result = users.emplace(
                std::make_pair(std::get<0>(m), UniqueUsers()));
            u = result.first;
        }
        u->second.emplace(std::get<1>(m));
    }

    // Create a min heap to hold the top-k highest scores.
    std::priority_queue<Score, std::vector<Score>, GreaterThanScore> topk;

    // Compute a similarity score for each unique pair of movies.
    std::unordered_set<MovieID> scored;
    for (const auto& m1 : users) {
        for (const auto& m2 : users) {
            if (m1.first == m2.first) {
                continue; // Same movie.
            }
            if (scored.count(m2.first) > 0) {
                continue; // Pair already scored.
            }
            auto score = similarity(m1.second, m2.second);
            if (topk.size() < k || std::get<2>(topk.top()) < score) {
                if (topk.size() == k) {
                    topk.pop();
                }
                topk.emplace(std::make_tuple(m1.first, m2.first, score));
            }
        }
        scored.emplace(m1.first);
    }

    // Copy and return the top-k elements from the heap.
    std::vector<Score> scores;
    scores.reserve(k);
    while (!topk.empty()) {
        scores.push_back(topk.top());
        topk.pop();
    }

    return scores;
}

TEST_CASE("examples", "[topkmovies]")
{
    MovieTable movies{
        {"m1", "u1"}, {"m1", "u2"}, {"m1", "u3"},
        {"m2", "u2"}, {"m2", "u3"}, {"m2", "u4"},
        {"m3", "u4"}, {"m3", "u5"}, {"m3", "u6"}, {"m3", "u7"},
        {"m4", "u5"}, {"m4", "u6"}, {"m4", "u7"}, {"m4", "u8"},
        {"m5", "u6"}, {"m5", "u7"}, {"m5", "u8"}, {"m5", "u9"},
        {"m5", "u10"},
        {"m5", "u11"},
        {"m5", "u12"}
    };
    std::size_t k = 3;

    CAPTURE(movies, k);

    std::vector<Score> expected_scores{
        {"m5", "m4", 0.375},
        {"m1", "m2", 0.5},
        {"m4", "m3", 0.6}
    };

    auto rcv = topk_similar_movies(movies, k);
    REQUIRE(rcv == expected_scores);
}
