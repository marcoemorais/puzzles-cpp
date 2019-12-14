#include <algorithm>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// Value is used to store a value and associated chunk in a heap.
template <typename T>
struct Value
{
    T value;
    std::size_t chunkid;
};

// GreaterThanValue orders Value by value.
template <typename T>
struct GreaterThanValue
{
    bool operator()(const Value<T>& lhs, const Value<T>& rhs)
    {
        return lhs.value > rhs.value;
    }
};

struct LoadChunkError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

namespace fs = std::filesystem; // Shorter alias.

// MwayMergesort is external sort with no more than k of m elements in memory.
template <typename T>
class MwayMergesort
{
  public:
    MwayMergesort(const std::string& infn,      // Input filename.
                  const std::string& outfn,     // Output filename.
                  std::size_t m,                // Number of integers to sort.
                  std::size_t k,                // Max integers in memory.
                  const fs::path& tmpdirn)      // Temp directory name.
        : infn(infn)
        , outfn(outfn)
        , m(m)
        , k(k)
        , p(m/k) // Assume even division.
        , q(k/p) // Assume even division.
        , tmpdirn(tmpdirn)
    { }

    ~MwayMergesort()
    {
        cleanup();
    }

    // sort sorts and writes the contents of infn to outfn.
    void sort()
    {
        // Open the input file.
        std::ifstream infs(infn);
        if (!infs) {
            throw std::system_error(errno, std::system_category(),
                                    "file: " + infn);
        }
        infs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        // Open the output file.
        std::ofstream outfs(outfn);
        if (!outfs) {
            throw std::system_error(errno, std::system_category(),
                                    "file: " + outfn);
        }
        outfs.exceptions(std::ofstream::failbit | std::ofstream::badbit);

        // Read, sort, and write input into p chunks of size k.
        splitAndSortChunks(infs);

        // Initialize a min heap of size k.
        initHeap();

        // Pull the minimum entry from the heap and write to output file.
        while (!min_heap.empty()) {
            Value<T> value = min_heap.top();
            outfs << value.value << "\n";
            min_heap.pop();
            // Increment the count of elements from the chunk.
            if ((++chunksrd[value.chunkid] % q) == 0) {
                // Load the next q entries from this chunk into the heap.
                if (chunksrd[value.chunkid] < (m/p)) {
                    loadChunk(value.chunkid);
                }
            }
        }
        outfs.flush();

        // Sanity check the number of elements read from heap.
        auto sum = std::accumulate(std::begin(chunksrd), std::end(chunksrd),
                                   std::size_t{0});
        if (sum != m) {
            throw LoadChunkError{
                std::string{"insufficient values read from heap"} +
                " expected: " + std::to_string(m) +
                " received: " + std::to_string(sum)
            };
        }
    }

  private:
    // infn is the input filename.
    std::string infn;

    // outfn is the output file.
    std::string outfn;

    // m is the total number of integers to sort.
    std::size_t m;

    // k is the maximum number of integers in memory at any time.
    std::size_t k;

    // p=m/k is the number of splits the input file will be divided into.
    std::size_t p;

    // q=k/p is the number of integers kept in memory from a single split.
    std::size_t q;

    // tmpdirn is the temporary directory name.
    fs::path tmpdirn;

    // chunksfn are the p chunks file names.
    std::vector<std::string> chunksfn;

    // chunksfs are the p chunks file streams.
    std::vector<std::fstream> chunksfs;

    // chunksrd is the number of elements read from the heap for each chunk.
    std::vector<std::size_t> chunksrd;

    // min_heap holds the next k values from p chunks.
    std::priority_queue<Value<T>,
                        std::vector<Value<T>>,
                        GreaterThanValue<T>> min_heap;

    void splitAndSortChunks(std::ifstream& infs)
    {
        // Create the temporary directory.
        std::error_code ec;
        fs::create_directories(tmpdirn, ec);
        if (ec) {
            throw std::system_error(ec, "directory: " + tmpdirn.string());
        }

        // Allocate an in-memory buffer used to sort each split.
        std::vector<T> chunk;
        chunk.reserve(k);
        std::size_t chunkid{0};

        // Read input into chunks of size k.
        for (T value; infs >> value; ) {
            chunk.emplace_back(value);
            // Sort each chunk and write the chunk to temporary output file.
            if (chunk.size() == k) {
                std::sort(std::begin(chunk), std::end(chunk));
                fs::path chunkfn = tmpdirn /
                    fs::path("chunk-" + std::to_string(chunkid));
                std::fstream chunkfs(chunkfn, std::ios_base::trunc
                                                | std::ios_base::in
                                                | std::ios_base::out);
                if (!chunkfs) {
                    throw std::system_error(errno, std::system_category(),
                                            "file: " + chunkfn.string());
                }
                chunkfs.exceptions(
                    std::fstream::failbit | std::fstream::badbit);
                std::copy(std::begin(chunk), std::end(chunk),
                          std::ostream_iterator<T>(chunkfs, "\n"));
                chunkfs.flush();
                chunkfs.seekg(0); // Rewind.
                chunk.clear(); // Purge values from current chunk.
                chunksfn.emplace_back(chunkfn);
                chunksfs.emplace_back(std::move(chunkfs)); // Owned by vector.
                if (++chunkid == p) {
                    break; // Last chunk.
                }
            }
        }

        if (chunkid != p) {
            throw LoadChunkError{
                std::string{"insufficient chunks read from input file"} +
                " expected: " + std::to_string(p) +
                " received: " + std::to_string(chunkid)
            };
        }
    }

    void initHeap()
    {
        // Load the p chunks of q entries each into the heap.
        for (std::size_t chunkid = 0; chunkid < p; ++chunkid) {
            loadChunk(chunkid);
            chunksrd.emplace_back(0);
        }

        if (min_heap.size() != k) {
            throw LoadChunkError{
                std::string{"insufficient values loaded into heap"} +
                " expected: " + std::to_string(k) +
                " received: " + std::to_string(min_heap.size())
            };
        }
    }

    void loadChunk(std::size_t chunkid)
    {
        std::size_t count{0};

        // Read the next q entries from the chunk into the heap.
        for (T value; (count < q) && (chunksfs[chunkid] >> value); ++count) {
            min_heap.emplace(Value<T>{value, chunkid});
        }

        if (count != q) {
            throw LoadChunkError{
                std::string{"insufficient values read from chunk"} +
                " chunkid: " + std::to_string(chunkid) +
                " expected: " + std::to_string(q) +
                " received: " + std::to_string(count) +
                " file: " + chunksfn[chunkid]
            };
        }
    }

    void cleanup()
    {
        // Remove the temporary directory and all its contents.
        std::error_code ec;
        fs::remove_all(tmpdirn, ec);
        if (ec) {
            throw std::system_error(ec, "directory: " + tmpdirn.string());
        }
    }
};

TEST_CASE("random", "[mwaymergesort]")
{
    using T = std::uint32_t;

    std::size_t m{100000};  // 100k
    std::size_t k{10000};   // 10k

    // Cleanup output files from previous tests.
    std::string outfn{"sortout"}, tmpdirn{"tmp"};
    {
        fs::remove(outfn);
        fs::remove_all(tmpdirn);
    }

    // Create the input file and fill with m random integers.
    std::string infn{"randin"};
    {
        std::ofstream outfs(infn, std::ios_base::trunc);
        if (!outfs) {
            throw std::system_error(errno, std::system_category(),
                                    "file: " + infn);
        }
        outfs.exceptions(std::ofstream::failbit | std::ofstream::badbit);

        std::mt19937 gen{std::random_device{}()};
        std::uniform_int_distribution<T> dis(INT32_MIN, INT32_MAX);

        for (std::size_t i = 0; i < m; ++i) {
            outfs << dis(gen) << "\n";
        }
        outfs.flush();
    }

    // Initialize the sort algorithm.
    MwayMergesort<T> sorter(infn, outfn, m, k, tmpdirn);

    // Sort the input.
    sorter.sort();

    // Read the output file and confirm that all m integers are sorted.
    {
        std::ifstream infs(outfn);
        if (!infs) {
            throw std::system_error(errno, std::system_category(),
                                    "file: " + outfn);
        }
        infs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        T prev_value;
        infs >> prev_value;

        std::size_t countrd{1};

        for (T value; countrd < m && infs >> value; prev_value = value,
                                                    ++countrd) {
            auto is_sorted = prev_value <= value;
            if (!is_sorted) {
                CAPTURE(countrd, prev_value, value);
            }
            REQUIRE(is_sorted == true);
        }

        CAPTURE(countrd, m);
        REQUIRE(countrd == m);
    }

    // Cleanup the input and output files.
    {
        fs::remove(infn);
        fs::remove(outfn);
    }
}
