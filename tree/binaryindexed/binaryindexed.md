# binaryindexed

---
## Problem Statement
A [binary indexed tree](https://en.wikipedia.org/wiki/Fenwick_tree)
aka Fenwick Tree is used to efficiently compute prefix sums over ranges of
elements equivalent to bins in a histogram. The structure supports the add and
cumsum operations using O(log n) time and no additional space.

Implement a binary indexed tree data structure with the following interface:
* Constructor which takes the total number of bins.
* `add(t, x)` adds the value x at bin t.
* `cumsum(t0, t1)` returns the cumulative sum of the values in bins [t0, t1].

The operations should satisfy the time and space requirements described above.

---
## Additional Details

Move forward to next parent in the tree from index i using the operation:
```
i + (i & (-i))
```

Move backward to next parent in the tree from index i using the operation:
```
i - (i & (-i))
```

---
## Examples

---
## Solution

For the add operation increment the bins in the tree by traversing from the
element starting at t and moving forward (one-level higher and to the right)
until you reach the end of the tree.

For the cumsum operation accumulate the sums of all bins by traversing from
the element start at t and moving backward (one-level higher and to the left)
until you reach the begining of the tree.

---
## References
This problem appears as Problem 11.1 in
<cite data-cite="miller2019daily">(Miller and Wu, 2019)</cite>.

```
@book{miller2019daily,
  title={Daily Coding Problem: Get Exceptionally Good at Coding Interviews by Solving One Problem Every Day},
  author={Miller, A. and Wu, L.},
  isbn={9781793296634},
  url={https://books.google.com/books?id=Fz29wQEACAAJ},
  year={2019},
  publisher={INDEPENDENTLY PUBLISHED}
}
```

