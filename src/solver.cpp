#include "fastwinding/solver.h"
#include "UT_SolidAngle.h"
#include "UT_FixedVector.h"
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace fastwinding {

class FastWindingSolver::Impl {
public:
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    std::vector<HDK_Sample::UT_Vector3T<float>> vertices_buffer;
};

FastWindingSolver::FastWindingSolver() : impl(std::make_unique<Impl>()) {}
FastWindingSolver::~FastWindingSolver() = default;

FastWindingSolver::FastWindingSolver(FastWindingSolver&&) noexcept = default;
FastWindingSolver& FastWindingSolver::operator=(FastWindingSolver&&) noexcept = default;

bool FastWindingSolver::init(const MeshObject& mesh, int order) {
    if (!mesh.isValid()) {
        return false;
    }

    // 1. Copy standard vertices into UT_Vector3T buffer
    const size_t num_vertices = mesh.vertices.size();
    impl->vertices_buffer.resize(num_vertices);
    for (size_t i = 0; i < num_vertices; ++i) {
        impl->vertices_buffer[i][0] = mesh.vertices[i][0];
        impl->vertices_buffer[i][1] = mesh.vertices[i][1];
        impl->vertices_buffer[i][2] = mesh.vertices[i][2];
    }

    // 2. Format face index array (UT_SolidAngle expects a flat array of ints)
    static_assert(sizeof(decltype(mesh.faces)::value_type) == 3 * sizeof(int),
        "std::array<int,3> must be layout-compatible with int[3] for reinterpret_cast");
    static_assert(alignof(decltype(mesh.faces)::value_type) == alignof(int),
        "std::array<int,3> alignment must match int alignment");
    const int* faces_ptr = reinterpret_cast<const int*>(mesh.faces.data());

    // 3. Initialize the internal SideFX solver
    impl->solid_angle.init(
        static_cast<int>(mesh.faces.size()),
        faces_ptr,
        static_cast<int>(num_vertices),
        impl->vertices_buffer.data(),
        order
    );
    return true;
}

float FastWindingSolver::compute(const std::array<float, 3>& query_point, double accuracy_scale) const {
    HDK_Sample::UT_Vector3T<float> Pp;
    Pp[0] = query_point[0];
    Pp[1] = query_point[1];
    Pp[2] = query_point[2];
    
    float solid = impl->solid_angle.computeSolidAngle(Pp, accuracy_scale);
    return solid / (4.0f * static_cast<float>(M_PI));
}

std::vector<float> FastWindingSolver::computeBatch(
    const std::vector<std::array<float, 3>>& query_points, 
    double accuracy_scale) const {
    
    std::vector<float> results(query_points.size());
    
    // Multi-core parallel calculation utilizing the system's TBB
    tbb::parallel_for(tbb::blocked_range<size_t>(0, query_points.size()), [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i != r.end(); ++i) {
            results[i] = compute(query_points[i], accuracy_scale);
        }
    });
    
    return results;
}

} // namespace fastwinding
