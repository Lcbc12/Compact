# Compact
Public Compact Proof of Retrievability Implementation
For development purpose only

## Version 1

Implementation of public compact proof of retrievability

- [x] s = 1
- [x] nb_challenge = 1
- [x] elements are pre-computed and copy/paste in the code
- [x] no code to generate all elements

Works with ganache configuration.

## Version 2

- [x] Implementation considering s >= 1 and nb_challenge >= 1
- [x] Code enabling elements generation
- [x] C++ code to generate all elements
- [ ] Generating elements creates JSON files in solidity/test/ directory
- [ ] Cleaning C++ code and handle exceptions
- [ ] Better scripts and extraction of benchmarks (CSV file)

Elements are generated thanks to the C++ [libff library](https://github.com/scipr-lab/libff).

I made the choice to use this library because C++ are way more efficient than Python.

I also use this program for benchmarks so the faster the language is the better.

The C++ program is saved in libff-master/libff/algebra/curves/tests directory
