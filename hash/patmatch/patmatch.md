# patmatch

---
## Problem Statement
Given an input sequence of size n and a pattern of size m, implement a function
to find the first occurence of a pattern in the sequence by making on average
O(n+m) comparisions between sequence and pattern instead of the O(nm) number
of comparisions made using brute force pattern matching.

---
## Additional Details
Think of a hash function as a polynomial of the form:
```
$$
p(x) = p_0 x^{n-1} + p_1 x^{n-2} + \cdots + p_{n-2} x + p_{n-1}
$$
where
* $x$ is the base of the number system in which the hashed values appear
* $p_0, p_1, \cdots, p_{n-1}$ are the values to hash
* $n$ is the number of values to hash
```

Horner's rule can be used to efficiently evaluate a polynomial of this form
without explicitly computing each base raised to a power.
```
$$
p(x) = p_{n-1} + x(p_{n-2} + x(\cdots + x(p_1 + p_0 x)))
$$
```

---
## Examples

Pattern at middle of sequence.
```
sequence : [0,1,2,3,4,5,6,7,8,9]
pattern  : [3,4,5,6]
match    : 3
```

Pattern at start of sequence.
```
sequence : [0,1,2,3,4,5,6,7,8,9]
pattern  : [0,1,2,3]
match    : 0
```

Pattern at end of sequence.
```
sequence : [0,1,2,3,4,5,6,7,8,9]
pattern  : [6,7,8,9]
match    : 6
```

---
## Solution
The brute force solution to the pattern matching problem requires you to
iterate over each of the n subsequences from \[0, n-m+1) comparing the m
values in the subsequence to the m characters in the input pattern.  Imagine
this comparison as a sliding window moving across the input sequence that
stops and compares the values in the window to the values in the pattern.

The [Rabin-Karp](https://en.wikipedia.org/wiki/Rabin%E2%80%93Karp_algorithm)
algorithm replaces the element-by-element subsequence comparison in the
sliding window with a hash value comparison.  Although the hash function still
requires each value in the subsequence formed from the input sequence to be
evaluated, the hash function is modified so that the polynomial described above
can be evaluated in constant time.
* Remove the contribution of the value to the immediate left of the window
  from the hash function by subtracting `$p_0 x^{n-1}$` where `$p_0$` is the
  value to remove.
* Add the contribution of the value in the rightmost position of the window
  to the hash function.  As the window slides, this value will be raised to
  successively higher powers through the application of Horner's rule.

Since the hash value comparison still might produce false positives, any
subsequence which has the same hash value as the pattern must be subject to
an additional value-by-value comparison.  Since the number of false positives
is expected to be rare in a random sequence, behavior that results in many
false positives during the pattern match is also rare.

---
## References

This problem and solution are inspired by section 3.7.2 of
<cite data-cite="Skiena:2008:ADM:1410219">(Skienna, 2008)</cite> which
describes Rabin-Karp algorithm for string matching via hashing.

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

