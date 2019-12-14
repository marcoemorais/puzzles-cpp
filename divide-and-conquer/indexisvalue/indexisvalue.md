# indexisvalue

---
## Problem Statement
Implement a function that given a sorted array of distinct integers returns
the range of indices for which the index matches value.  In other words,
return the list of indices, i, from the array A such that A[i] = i.

---
## Additional Details

The range of indices where A[i] = i must be contiguous.
* If A[i] > i, then there exists no element A[m] = m for all m > i.
* If A[i] < i, then there exists no element A[n] = n for all n < i.

---
## Examples

Run of matching indices in middle.
```
input : [-10, -5, 0, 3, 4, 5, 7, 20]
output: [3, 4, 5]
```

Run of matching indices in left subarray.
```
input : [0, 1, 2, 5, 7, 8, 9, 10]
output: [0, 1, 2]
```

Run of matching indices in right subarray.
```
input : [-10, -5, 0, 1, 4, 5, 10]
output: [4, 5]
```

---
## Solution

Use divide and conquer to partition the array into subarrays where index
matches value.

* Check the midpoint, k, of the subarray formed from [first, last) as follows.
    * If A[k] = k, then you are in the middle of a run of index matches value.
        * Scan left from the midpoint to find the start of the run.
        * Scan right from the midpoint to find the end of the run.
        * Return the indices from [start, end).
    * If A[k] > k, then recursively search the left subarray from [first, k).
    * If A[k] < k, then recursively search the right subarray from [k+1, last).
* Terminate the recursion if the subarray is empty.

---
## References

This problem appears as Problem 3.3 of
<cite data-cite="roughgarden2017algorithms">(Roughgarden, 2017)</cite>.

```
@book{roughgarden2017algorithms,
  title={Algorithms Illuminated (Part 1): The Basics},
  author={Roughgarden, T.},
  isbn={9780999282908},
  lccn={2017914282},
  series={Algorithms Illuminated},
  url={https://books.google.com/books?id=W6xDtAEACAAJ},
  year={2017},
  publisher={Soundlikeyourself Publishing, LLC}
}
```

