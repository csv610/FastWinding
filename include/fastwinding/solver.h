#pragma once
#include "mesh_object.h"
#include <vector>
#include <array>
#include <memory>
#include <ranges>
#include <concepts>

namespace fastwinding {

/**
 * @class FastWindingSolver
 * @brief High-performance solver for generalized winding numbers.
 * 
 * This class provides an efficient implementation of the Fast Winding Number algorithm,
 * utilizing a SIMD-optimized BVH for acceleration and multi-core batch processing.
 */
class FastWindingSolver {
public:
    /**
     * @brief Constructs a new FastWindingSolver instance.
     */
    FastWindingSolver();

    /**
     * @brief Destructor.
     */
    ~FastWindingSolver();
    
    // Non-copyable for safety with internal resources
    FastWindingSolver(const FastWindingSolver&) = delete;
    FastWindingSolver& operator=(const FastWindingSolver&) = delete;

    /**
     * @brief Move constructor.
     */
    FastWindingSolver(FastWindingSolver&&) noexcept;

    /**
     * @brief Move assignment operator.
     */
    FastWindingSolver& operator=(FastWindingSolver&&) noexcept;

    /**
     * @brief Initializes the solver with a mesh.
     * 
     * @param mesh The input triangular mesh.
     * @param order The expansion order for the approximation (default is 2).
     * @return true if initialization was successful.
     */
    bool init(const MeshObject& mesh, int order = 2);

    /**
     * @brief Computes the winding number at a single query point.
     * 
     * @param query_point The 3D point to evaluate.
     * @param accuracy_scale Scaling factor for approximation accuracy (default is 2.0).
     * @return The computed winding number (typically 1.0 inside, 0.0 outside).
     */
    [[nodiscard]] float compute(const std::array<float, 3>& query_point, double accuracy_scale = 2.0) const;

    /**
     * @brief Computes winding numbers for a batch of query points in parallel.
     * 
     * This method utilizes Intel TBB to distribute calculations across all available cores.
     * 
     * @tparam Range A contiguous range of 3-element float arrays.
     * @param query_points The collection of points to evaluate.
     * @param accuracy_scale Scaling factor for approximation accuracy (default is 2.0).
     * @return A vector containing the winding number for each query point.
     */
    template<typename Range>
    requires std::ranges::contiguous_range<Range> && 
             std::same_as<std::ranges::range_value_t<Range>, std::array<float, 3>>
    [[nodiscard]] std::vector<float> computeBatch(const Range& query_points, 
                                                double accuracy_scale = 2.0) const {
        const size_t n = std::ranges::size(query_points);
        std::vector<float> results(n);
        
        // Internal batch processing implementation
        computeBatchImpl(
            n, 
            std::ranges::data(query_points), 
            results.data(), 
            accuracy_scale
        );
        
        return results;
    }

private:
    /**
     * @brief Internal implementation of batch processing to keep the header clean.
     */
    void computeBatchImpl(size_t n, 
                         const std::array<float, 3>* queries, 
                         float* results, 
                         double accuracy_scale) const;

    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace fastwinding
