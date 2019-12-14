# urngfromflip

---
## Problem Statement
You are working on a system whose only source of randomness is a very high
speed coin flipping device that returns 0 or 1 with equal probability.

Implement a random number generator for this system using the high speed coin
flipper that returns a random integer in the range [a, b] where each value
returned from the generator is equally likely.

Bonus: Propose a method to validate the generator produces values which are
uniformly distributed.

---
## Additional Details
The chi-squared value of a histogram of k random trials is given by:
```
$$
\chi^2 = \sum_{i=1}^n \frac{(x_i - k/n)^2}{k/n}
$$
where
* $n$ is in the range, $b-a+1$, of integer values returned from the generator
* $x_i$ is the count from the histogram for bin with right edge $a+i$
* $k/n$ is the expected count for any bin in the histogram
```

The null hypothesis, H0, is that the numbers returned from the generator are
from a uniform distribution. If the value of the chi-squared is small relative
to a test statistic chosen based on the degrees of freedom n-1 and level of
significance, then we fail to reject the null hypothesis and are satisifed
with the claim that the values returned from the generator are uniformly
distributed.

---
## Examples

---
## Solution

* Use the high speed coin flipper to build an integer in range b-a bit-by-bit.
    * Compute the range n = b - a of the random values to return.
    * Compute the number of bits m required to represent n as ceil(lg2 n).
    * Obtain m bits from the high speend coin flipper and convert to decimal.
    * If the decimal value urand is outside the range, obtain another m bits.
    * Else return the value a + urand as the random number.

---
## References


