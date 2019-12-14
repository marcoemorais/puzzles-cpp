# staircase

---
## Problem Statement
Implement a function that given some height of a staircase, named h, and set
of possible steps, named S, returns all of the unique ways to climb the
staircase.

---
## Additional Details
Alternative formulation: Find all permutations of elements from S that sum to h.

---
## Examples
Solve for the values of h and S given below.
```
h = 4
S = {1, 2, 3}
```

k-permutations.
```
k=4: {1111}
k=3: {211,121,121}
k=2: {22,31,13}
k=1: {}
```

---
## Solution
* Use dynamic programming to recursively solve the h-i staircase subproblem
  for i=1..S.
    * Let s represent the staircase function.
    * At each level of recursion iterate from i=1..S and compute `s(h-i)+s(i)`.
        * Concatenate the pair of results obtained from each subproblem.
            * `(sum1, sum2)`
            * `(sum2, sum1)`
        * Handle cases where height is reachable in a single step without sum.
    * Base case of the recursion `s(0)` returns `{}`.

Example stack trace.
```
s(h=3, S=[1, 2, 3])
    i=1 => s(h=2) + s(h=1)
        ...
    i=2 => s(h=1) + s(h=2)
        ...
    i=3 => s(h=0) + s(h=3)
        ...
```

---
## References

This problem appears as Problem 13.1 in
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

