# stepword

---
## Problem Statement
Implement a function that given a pair of words and dictionary of valid words,
finds the shortest path of one character transformations between the pair
of words.  If no path is possible, then return the empty path.

---
## Additional Details

In order for a path to exist, both words must be in the dictionary.

---
## Examples

Example stepword chain with path.
```
input pair   : ( dog, cat )
dictionary   : { dog, dot, dop, dat, cat }
shortest path: ( dog, dot, dat, cat )
```

Example stepword chain without path.
```
input pair   : ( dog, cat )
dictionary   : { dog, dot, tod, mat, cat }
shortest path: ( )
```

---
## Solution
Treat this as finding shortest path through an unweighted undirected graph.

The words in the dictionary are the vertices and an edge exists between two
words when there is exactly one character difference between the words.

Rather than explicitly building the graph before finding the shortest path,
edges are discovered during the traversal by comparing a word to every other
word in the dictionary to see whether they differ by exactly one character.

A vector of parent words is used to record the traversal and that vector is
iterated over in reverse to return the path from the first word in the pair
to the second.

---
## References


