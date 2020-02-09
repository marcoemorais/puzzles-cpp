# randstream

---
## Problem Statement
Given a stream of elements too large to store in memory pick k random elements
from the stream with uniform probability.

Bonus: Propose a method to validate the random sample produces values which
are uniformly distributed.

---
## Additional Details
The chi-squared value of a histogram of n bins is given by:
```
$$
\chi^2 = \sum_{i=1}^n \frac{(x_i - T*k/n)^2}{T*k/n}
$$
where
* $k$ is the number of samples to return from the stream
* $n$ is in the the size of the stream
* $T$ is the number of trials
* $x_i$ is the count from the histogram for bin with right edge $a+i$
* $T*k/n$ is the expected count for any bin in the histogram
```

The null hypothesis, H0, is that the samples are selected from the stream
with uniform probability.  If the value of the chi-squared is small relative
to a test statistic chosen based on the degrees of freedom n-1 and level of
significance, then we fail to reject the null hypothesis and are satisifed
with the claim that the samples are drawn from a uniform distribution.

---
## Examples

---
## Solution
Use [Reservoir sampling](https://en.wikipedia.org/wiki/Reservoir_sampling)
algorithm described as follows.
1. Fill a reservoir with the first k elements read from the stream.
2. For each additional element read from the stream, pick a random number
from [1, n] where n is the total number of elements read from the stream so
far.
3. If the random number is less than or equal to k, then replace the element
at position k-1 with the element read from the stream.

---
## References

