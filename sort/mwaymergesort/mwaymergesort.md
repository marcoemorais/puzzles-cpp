# mwaymergesort

---
## Problem Statement
Sort an input file of 100k unsorted 32-bit signed integers without keeping
more than 10k integers in memory at any time.  Write the sorted output to a
new file.

Describe the worst case runtime performance of the solution.

---
## Additional Details

---
## Examples

---
## Solution
Parameterize the solution with some variables.
    * m is the total number of integers to sort (100k)
    * k is the maximum number of integers in memory at any time (10k)
    * p=m/k is the number of splits the input file will be divided into (10)
    * q=k/p is the number of integers kept in memory from a single split (1k)

1. Read input file into p (10) chunks of size k (10k).
2. Sort each chunk and write the sorted chunk to an output temporary file.
3. Initialize a min heap of size k (10k).
4. Read the first q (1k) entries from the p (10) temporary files into the heap.
    * Add something to each heap entry which identfies the temporary file from
      which the integer is obtained.
    * Initialize an array of size p (10) with the count of integers from each
      temporary file currently in the heap.  Set all entries to q (1k).
5. Pull the minimum entry from the heap and write to the output solution file.
    * Decrement the count of integers in the heap for the temporary file from
      which the integer was obtained.
    * When the count for any temporary file reaches 0, then load the next
      q (1k) entries from the temporary file and reset the counter to q (1k).
6. Repeat step 5 until the heap is empty.

The worst case runtime performance can be described as:
```
$$
p (k \log_2 k) + m \log_2 m + m
$$
where
* $p (k \log_2 k)$ is the cost of sorting the temporary files
* $m \log_2 m$ is the cost of inserting the m elements into the min heap
* $m$ is the cost of removing the min element m times from the min heap
```

The solution is expected to contain a lot of additional overhead compared to
a pure in-memory solution, since we are replenshing the heap with elements
being read from a file.

In order to fully satisfy the requirement on the maximum number of integers
in memory, the sorting algorithm used for the temporary files should require
no additional storage.  This eliminates mergesort, but quicksort satisfies.

--
## References

This problem and solution are inspired by section 4.8 of
<cite data-cite="Skiena:2008:ADM:1410219">(Skienna, 2008)</cite> which
mentions use of multiway mergesort for external sorting.

```
@book{Skiena:2008:ADM:1410219,
 author = {Skiena, Steven S.},
 title = {The Algorithm Design Manual},
 year = {2008},
 isbn = {1848000693, 9781848000698},
 edition = {2nd},
 publisher = {Springer Publishing Company, Incorporated},
}
```

