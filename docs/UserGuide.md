# Fast Winding Numbers - User Guide

**For Undergraduate Students**

This guide will walk you through building, running, and using the Fast Winding Number library.

---

## Table of Contents
1. [What is a Winding Number?](#what-is-a-winding-number)
2. [Building the Project](#building-the-project)
3. [Running the Program](#running-the-program)
4. [Understanding the Output](#understanding-the-output)
5. [Running Tests](#running-tests)
6. [Running Benchmarks](#running-benchmarks)
7. [Code Overview](#code-overview)
8. [Troubleshooting](#troubleshooting)

---

## What is a Winding Number?

A **winding number** tells you how many times a curve wraps around a point. In 3D graphics, we use it to determine if a point is inside or outside a mesh.

**Key Formula:**
```
Winding Number = (1/4π) × Σ(solid_angle of each triangle)
```

- If WN = 1.0: Point is inside the mesh
- If WN = 0.0: Point is outside the mesh

---

## Building the Project

### Prerequisites
Make sure you have installed:
- CMake (version 3.20 or higher)
- A C++20 compatible compiler
- Eigen3
- TBB (Threading Building Blocks)

### Build Steps

```bash
# Navigate to the project directory
cd FastWinding

# Create a build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . -j4
```

**That's it!** 🎉 You should see:
```
[100%] Built target fastwinding
```

---

## Running the Program

### Basic Usage

```bash
./fastwinding <input_mesh> <query_points> <output_file>
```

**Arguments:**
| Argument | Description | Example |
|----------|-------------|---------|
| input_mesh | 3D mesh file (PLY, OBJ, OFF, STL) | `pig-head.ply` |
| query_points | Nx3 matrix of points to query | `query.dmat` |
| output_file | Output file for winding numbers | `output.dmat` |

### Example

```bash
./fastwinding pig-head.ply pig-head-Q.dmat output.dmat
```

This will:
1. Load the pig-head mesh (2,789 vertices, 5,472 triangles)
2. Process 125,000 query points
3. Save winding numbers to output.dmat

---

## Understanding the Output

The output is a DMAT file containing winding numbers:

```
0 0
1 125000
0.5
0.3
0.0
...
```

**Format:**
- Line 1: `0 0` (header)
- Line 2: `1 125000` (1 row, 125000 columns)
- Remaining lines: Winding numbers (one per query point)

**Interpretation:**
| Value | Meaning |
|-------|---------|
| ≈ 1.0 | Point is inside the mesh |
| ≈ 0.0 | Point is outside the mesh |
| 0.5 | Point is on the surface |

---

## Running Tests

We have unit tests and integration tests to verify correctness.

```bash
# Run all tests
cd build
ctest --output-on-failure
```

**Expected Output:**
```
100% tests passed, 0 tests failed out of 2
```

### Running Unit Tests Only

```bash
cd build
./unit_tests
```

This runs 15 unit tests covering:
- Math functions (sqrt, abs, trig)
- Vector operations
- Array operations
- Winding number logic

---

## Running Benchmarks

### General Benchmark

```bash
cd build
./benchmark
```

This tests:
- Mesh loading time
- BVH initialization time
- Query performance (serial vs parallel)
- Accuracy scales

**Sample Output:**
```
--- Serial vs Parallel ---
Serial 50000 queries in 65.12 ms
Parallel 50000 queries in 15.96 ms
```

### Scaling Benchmark

```bash
cd build
./benchmark_scaling
```

This shows how performance scales with mesh size:

| Mesh | Faces | Init (ms) | Query (ms) |
|------|-------|-----------|------------|
| Sphere_256 | 131K | 19.7 | 2.19 |
| Sphere_512 | 524K | 60.4 | 1.41 |
| Sphere_1024 | 2.1M | 205.4 | 1.62 |

---

## Code Overview

### Key Files

| File | Description |
|------|-------------|
| `main.cpp` | Main program entry point |
| `UT_SolidAngle.h` | Solid angle computation |
| `UT_BVH.h` | Bounding Volume Hierarchy |
| `VM_SIMD.h` | SIMD vector operations |
| `bench/bench_main.cpp` | General benchmarks |

### Using the Library in Your Code

```cpp
#include "WindingNumber/UT_SolidAngle.h"

// 1. Create solid angle calculator
HDK_Sample::UT_SolidAngle<float, float> solid_angle;

// 2. Initialize with your mesh
solid_angle.init(ntriangles, indices, nvertices, positions, 2);

// 3. Compute winding number for a query point
float wn = solid_angle.computeSolidAngle(query_point, 2.0f) / (4.0f * std::numbers::pi);
```

### Parameters

- **order**: Integration order (typically 2)
- **accuracy_scale**: Controls accuracy vs speed
  - `1.0`: Fast, ~1% error
  - `2.0`: Balanced, ~0.1% error (recommended)
  - `4.0`: Accurate, ~0.01% error

---

## Troubleshooting

### Build Errors

**"Could not find Eigen3"**
```bash
# Install via Homebrew (macOS)
brew install eigen

# Install via apt (Linux)
sudo apt-get install libeigen3-dev
```

**"Could not find TBB"**
```bash
# Install via Homebrew (macOS)
brew install tbb

# Install via apt (Linux)
sudo apt-get install libtbb-dev
```

### Runtime Errors

**"USAGE: ./fastwinding input.ply query.dmat output.dmat"**
- You're not providing enough arguments

**"Could not read file"**
- Check that the input files exist and are readable

### Performance Issues

**Query is too slow?**
- Try increasing accuracy_scale to 1.0 (faster but less accurate)
- Use parallel queries with igl::parallel_for

---

## Learning Exercises

### Exercise 1: Modify Accuracy
Try changing the accuracy_scale parameter in main.cpp:
```cpp
float accuracy_scale = 1.0;  // Faster
float accuracy_scale = 4.0;  // More accurate
```

### Exercise 2: Add Your Own Test
Add a new test case in `tests/unit_tests.cpp`:
```cpp
TEST(my_new_test) {
    // Add your test logic here
    ASSERT_TRUE(true);
}
```

### Exercise 3: Profile Your Code
Use Instruments (macOS) or gprof (Linux) to profile the benchmark and find bottlenecks.

---

## References

- **Original Paper**: Barill et al., "Fast Winding Numbers for Soups and Clouds", SIGGRAPH 2018
- **Eigen**: http://eigen.tuxfamily.org/
- **TBB**: https://github.com/oneapi-src/oneTBB
- **libigl**: https://libigl.github.io/

---

## FAQ

**Q: Why is query time independent of mesh size?**
A: The BVH (Bounding Volume Hierarchy) provides O(log n) lookup, so larger meshes don't slow down queries significantly.

**Q: What SIMD instructions are supported?**
A: ARM NEON (Apple Silicon, mobile) and x86 SSE (Intel/AMD desktop).

**Q: Can I use this for point clouds?**
A: This implementation is for triangle soups only, not point clouds.

---

**Happy Coding!** 🚀