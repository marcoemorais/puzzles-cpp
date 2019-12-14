# spiral2d

---
## Problem Statement
Implement a function that visits every cell of 2d matrix in a clockwise spiral.

---
## Additional Details

---
## Examples

Example input 4x4 matrix.
```
1       2       3       4
5       6       7       8
9       10      11      12
13      14      15      16
```

Expected clockwise spiral traversal starting from top-left corner.
```
1,2,3,4,8,12,16,15,14,13,9,5,6,7,11,10
```

---
## Solution

* The traversal is dictated by current position and direction of travel.
    * Current position is given by the row index i and column index j.
    * Direction of travel is an enum { right, down, left, up }.
    * A single iteration of the traversal algorithm increments row or column
      based on the value of the direction of travel.
    * Direction of travel changes when reaching a the right/down/left/up limit
      of travel threshold.
    * Limit of travel thresholds keep shrinking to produce an inward spiral.
* Maintain a counter hold the number of cells visited.
* Repeat the traversal algorithm until the number of cells visited equals the
  number of cells in the matrix.

--
## References


