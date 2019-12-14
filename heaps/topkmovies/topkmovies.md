# topkmovies

---
## Problem Statement
Implement a function that given a table of movie attendance data of the form
(movie, userid) returns the top-k movies which are most similar to each other
based on common users.  Your function should return a list of values of the
form (movie1, movie2, score) in ascending order by score.

---
## Additional Details

Use intersection over union
([Jaccard](https://en.wikipedia.org/wiki/Jaccard_index) to compute the
similarity between sets.

For example, given
    movie1 : { user1, user2, user3 }
    movie2 : { user2, user3, user4 }
    J(movie1, movie2) =  2 / 4 = 0.5

---
## Examples

---
## Solution

Preprocess the data so that you have a map from each movie to the set of users
which attended that movie.

For each unique pair of movies compute the similarity between the pair using
intersection over union.

Use a min heap to maintain the top-k highest similarity scores.

Copy and return the elements from the heap in a vector.

--
## References


