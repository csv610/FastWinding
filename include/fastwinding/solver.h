#pragma once
#include "mesh_object.h"
#include <vector>
#include <array>
#include <memory>

namespace fastwinding {

class FastWindingSolver {
public:
    FastWindingSolver();
    ~FastWindingSolver();
    
    // Non-copyable for safety with internal resources, but moveable
    FastWindingSolver(const FastWindingSolver&) = delete;
    FastWindingSolver& operator=(const FastWindingSolver&) = delete;
    FastWindingSolver(FastWindingSolver&&) noexcept;
    FastWindingSolver& operator=(FastWindingSolver&&) noexcept;

    // Initialize with a MeshObject
    bool init(const MeshObject& mesh, int order = 2);

    // Compute winding number for a single query point
    float compute(const std::array<float, 3>& query_point, double accuracy_scale = 2.0) const;

    // Compute winding numbers for a batch of query points in parallel (internally uses TBB)
    std::vector<float> computeBatch(const std::vector<std::array<float, 3>>& query_points, 
                                    double accuracy_scale = 2.0) const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace fastwinding
