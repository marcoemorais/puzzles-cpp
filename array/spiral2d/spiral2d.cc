#include <cstddef>
#include <cstdint>
#include <vector>

// Let Catch provide main().
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

// Vector is an alias for column vector.
template <typename T> using Vector = std::vector<T>;

// Matrix is an alias for matrix of column vectors.
template <typename T> using Matrix = std::vector<Vector<T>>;

// MatrixVisitorNoOp is a no-op visitor functor.
template <typename T>
struct MatrixVisitorNoOp
{
    void operator()(const Matrix<T>& m, std::size_t i, std::size_t j)
    { }
};

// spiral2d visits every cell of the matrix in a clockwise spiral.
template <typename T, typename MatrixVisitor>
void
spiral2d(const Matrix<T>& m, MatrixVisitor& visitor)
{
    std::size_t nrow = m.size(), ncol = m[0].empty() ? 0 : m[0].size();
    std::size_t ncells = nrow*ncol, visited = 0;

    // i,j index row and column.
    std::size_t i = 0, j = 0;

    // move is the direction of travel along the spiral.
    enum direction { right, down, left, up };
    direction move = right;

    // leftlim, rightlim, downlim, and uplim are limits of travel.
    // limits are dynamically adjusted during traversal to spiral inward.
    size_t leftlim = 0, rightlim = ncol-1,
           downlim = nrow-1, uplim = 0;

    // Keep iterating until every cell is visited.
    while (visited != ncells) {

        // Visit the cell.
        visitor(m, i, j);

        // Increment the indices and toggle direction.
        if (move == right) {
            if (j == rightlim) {
                move = down;
                i += 1;
                uplim = i;
            } else {
                ++j;
            }
        } else if (move == down) {
            if (i == downlim) {
                move = left;
                j -= 1;
                rightlim = j;
            } else {
                ++i;
            }
        } else if (move == left) {
            if (j == leftlim) {
                move = up;
                i -= 1;
                downlim = i;
            } else {
                --j;
            }
        } else if (move == up) {
            if (i == uplim) {
                move = right;
                j += 1;
                leftlim = j;
            } else {
                --i;
            }
        }

        ++visited;
    }
}

// MatrixVisitorVisitOrder remembers the visit order of a traversal.
template <typename T>
struct MatrixVisitorVisitOrder
{
    void operator()(const Matrix<T>& m, std::size_t i, std::size_t j)
    {
        visit_order.emplace_back(m[i][j]);
    }

    std::vector<T> visit_order;
};

TEST_CASE("examples", "[spiral2d]")
{
    using T = std::uint32_t;

    struct test_case
    {
        Matrix<T> m;
        std::vector<T> expected;
    };

    std::vector<test_case> test_cases{
        // Spiral over 2x2 square matrix.
        {
            { {1,2}, {3,4} },
            {1,2,4,3}
        },
        // Spiral over a 4x4 square matrix.
        {
            { {1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16} },
            {1,2,3,4,8,12,16,15,14,13,9,5,6,7,11,10}
        },
        // Spiral over a 4x3 rectangular matrix.
        {
            { {1,2,3}, {4,5,6}, {7,8,9}, {10,11,12} },
            {1,2,3,6,9,12,11,10,7,4,5,8}
        }
    };

    for (const auto& c : test_cases) {
        MatrixVisitorVisitOrder<T> visitor;
        spiral2d(c.m, visitor);
        CAPTURE(c.m, visitor.visit_order, c.expected);
        REQUIRE(visitor.visit_order == c.expected);
    }
}
