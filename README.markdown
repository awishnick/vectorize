## About *vectorize*
  *vectorize* is a library for generating reasonably good vectorized code
without manually writing intrinsics, handling loop boundaries, etc.
It cannot handle all classes of algorithms, only those that can be
represented by a for loop with no dependencies on any values other than
that at the current index, like taking the square root of every value in
an array. In the future it will also represent reductions, like adding up
every value in an array or finding the maximum.

  It is not an attempt to compete with hand-rolled functions. It is meant
to enable some of the "low-hanging fruit" to be vectorized very easily.
Often the choice is to quickly write a plain loop in C, or
to spend some time writing intrinsics to vectorize it. *vectorize* presents
a third option: write a loop that is as simple to write and read as the
C version, but whose performance is much closer to the hand-rolled version.

## Examples
    apply(n, inputs, outputs, sqrt(_x));
    apply(n, inputs, outputs, max(_x, 1.0f) - 1.0f);

  Notice how the code is about as easy to read as the C equivalent:

    for (unsigned i=0; i<n; ++i)
      outputs[i] = sqrt(inputs[i]);
    for (unsigned i=0; i<n; ++i)
      outputs[i] = max(inputs[i], 1.0f) - 1.0f;

## Short introduction
  The core of *vectorize* is the `apply` function. It is similar to the STL
algorithm `transform`. Its function signature is:

    template <class F>
    void apply(unsigned n, const float* src, float* target, F f);

The trick is that `f` is an expression whose type encodes the computation.
I'll call it the "computation kernel." For example, `sqrt(_x)` doesn't
compute anything, it just generates a type that knows how to compute the
square root of vectors and scalars. The `apply` function knows how to process
vectors of data using this computation kernel, and how to handle the remainder
as scalars if the size isn't evenly divisible by the vector width.
