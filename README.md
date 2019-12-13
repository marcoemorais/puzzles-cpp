# puzzles-cc

This repo contains coding puzzles of the sort that might appear in a book on algorithms and data structures or recreational mathematics.  Apart from the challenge and enjoyment of solving these problems, I created the repo to demonstrate concise self-contained examples of code that conforms to the [C++17 standard](https://en.wikipedia.org/wiki/C%2B%2B17).

Puzzles are organized into separate directories each containing a single source file with the solution to a puzzle.  Each source file is completely self-contained and depends only on access to the [C++ standard template library](https://en.wikipedia.org/wiki/Standard_Template_Library).  Test cases using the [Catch2](https://github.com/catchorg/Catch2) framework are also included in each source file.  Finally, each directory also contains a markdown file that summarizes the problem statement, adds canonical examples, and provides a human readable description of the solution.

A Dockerfile that contains a complete toolchain sufficient to build and run the puzzles is included.  In keeping with the minimal nature of the repo, I provide Makefiles to build and run the docker image, code, and associated tests.

In summary, I started this repo primarily for my own benefit as well as to be able to share examples of working code with other people when discussing modern C++.  Hope that you find this helpful.

