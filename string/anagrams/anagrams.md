# anagrams

---
## Problem Statement
Given a word w and string s find starting indices of all anagrams of s in w.

---
## Additional Details
An anagram is a word or phrase formed by rearranging the letters of a different
word or phrase, typically using all the original letters exactly once. For
example, `adobe` can be rearranged into `abode`.

---
## Examples
One anagram at start of word followed by 2 overlapping anagrams.
```
word    : abxaba
s       : ab
indices : [0, 3, 4]
```

Corner case with same characters but different frequency.
```
word    : ttwwt
s       : wwt
indices : wwt
```

---
## Solution
The brute force solution compares the sorted contents of a sliding window of
length equal to size of s to a sorted copy of s.

A more efficient method compares a character frequency map compiled from the
characters in the sliding window with the same map constructed from s.  In the
case of a positive match, the comparison requires iterating over all of the
keys in the map.

The key comparison can be replaced with an empty map comparison by defining
the entries in the map to have the following meanings:
* 0  = character appears in window and s
* &gt;0 = abundance of characters from s in window
* &lt;0 = character not in s appears in window

Given the invariants described above, an anagram will only have 0-valued
entries in the map.  Rather than checking whether all entries are 0-valued,
we remove 0-valued entries from the map so that the anagram test reduces to
checking whether map is empty.

---
## References

This problem appears as Problem 2.1 in
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

