# MAT(rix)

A header-only, multi-dimensional matrix library for CUDA and C++.
Provides a template-based matrix structure with sizes defined at compile-time.

## Features

- **CUDA Compatible**: All methods are marked `__host__ __device__`.
- **Compile-time Sizing**: Rows and columns are template parameters.
- **Precision**: Equality tests use a default epsilon of `2e-6`.
- **Determinants**: Supports square matrices using Laplace expansion.

## Method Signatures

### Constructors and Basic Ops

- `mat()`: Default constructor (uninitialized data).

- `operator+`, `operator+=`: Matrix addition.

- `operator-`, `operator-=`: Matrix subtraction.

- `operator==`: Equality test with tolerance.

### Multiplication

- `operator*(float)`: Scalar Multiplication

- `operator*=(float)`: Scalar multiplication assignment.

- `operator*(vec<c>)`: Matrix-vector transformation (returns `vec<r>`).

- `operator*(mat)`: Matrix-matrix multiplication (requires `c1 == r2`).

### Matrix Operations

- `transpose()`: Returns the transposed matrix.
(Note: currently implemented for square matrices).

- `transpose_inplace()`: Transposes the matrix in-place (requires square matrix).

- `get_minor(int row, int col)`: Returns a minor matrix of size `(r-1, c-1)`,
excluding row `row` and column `col`.

- `det()`: Returns the determinant of a square matrix.

## Performance Warning

The `det()` method uses a recursive **Laplace expansion** algorithm O(n!).
It is intended for small matrices (e.g., 2x2, 3x3, 4x4) used in graphics and physics.
Do not use this for large matrices, as it will cause significantp
performance degradation or stack overflow.

## To-Be-Implemented (TBI)

- Matrix Inversion
- LU Decomposition
