# Fast Winding Number Library

## Overview
High-performance library for computing winding numbers of triangular meshes using SIMD-optimized BVH acceleration.

## Algorithm Complexity Analysis

### Initialization
- **Time**: O(n log n) where n = number of triangles
- **Memory**: O(n) for BVH storage
- The BVH is built using a surface area heuristic (SAH) with 4-way branching

### Query
- **Single Query**: O(log n + k) where k = number of intersected triangles
- **Batch Query**: O(m log n + total_intersections) for m queries
- Query time is independent of mesh size due to BVH acceleration

### SIMD Performance
- **ARM NEON**: 4x float operations per cycle
- **x86 SSE**: 4x float operations per cycle
- Performance scales linearly with mesh size for initialization

## Memory Usage
- BVH nodes: ~16 bytes per node
- Triangle data: 12 bytes per triangle (3 x float3)
- Total: ~28 bytes per triangle + O(n log n) BVH overhead

## Accuracy
The `accuracy_scale` parameter controls subdivision depth:
| Scale | Subdivisions | Relative Error |
|-------|-------------|----------------|
| 1     | 1x         | ~1%           |
| 2     | 4x         | ~0.1%         |
| 4     | 16x        | ~0.01%        |
| 8     | 64x        | ~0.001%       |

## Thread Safety
- Read-only access after init()
- Safe for parallel query processing via TBB
- No locks required during query phase

## Performance Benchmarks
- 2M faces init: ~200ms
- 2M faces query (10k points): ~16ms
- Per-query: ~1.6µs (independent of mesh size)

## API Reference

### UT_SolidAngle
```cpp
template<typename T, typename S>
class UT_SolidAngle {
    void init(int ntriangles, const int* indices, 
              int nvertices, const UT_Vector3T<T>* positions, int order);
    
    S computeSolidAngle(const UT_Vector3T<T>& query_point, 
                        const T accuracy_scale = T(2.0)) const;
};
```

### Usage
```cpp
UT_SolidAngle<float, float> solid_angle;
solid_angle.init(ntriangles, indices, nvertices, positions, 2);

float wn = solid_angle.computeSolidAngle(query, 2.0) / (4.0 * std::numbers::pi);
```

## Build Requirements
- CMake 3.20+
- C++20
- Eigen3
- TBB

## Platform Support
- x86_64 (SSE/AVX)
- ARM64 (NEON)