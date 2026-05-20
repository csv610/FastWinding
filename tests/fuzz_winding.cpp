#include <cstdint>
#include <cstddef>
#include <vector>
#include <Eigen/Dense>
#include "WindingNumber/FastWindingNumbers.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 12) return 0;

    auto safe_read = [&](size_t offset, auto& out, size_t count) {
        if (offset + count > size) return false;
        std::memcpy(&out, data + offset, count);
        return true;
    };

    uint32_t num_verts, num_faces, num_queries;
    if (!safe_read(0, num_verts, 4)) return 0;
    if (!safe_read(4, num_faces, 4)) return 0;
    if (!safe_read(8, num_queries, 4)) return 0;

    const uint32_t max_limit = 10000;
    if (num_verts > max_limit || num_faces > max_limit || num_queries > max_limit) return 0;

    size_t expected = 12 + num_verts * 12 + num_faces * 12 + num_queries * 12;
    if (size < expected) return 0;

    size_t offset = 12;

    Eigen::MatrixXd V(num_verts, 3);
    for (uint32_t i = 0; i < num_verts; ++i) {
        float x, y, z;
        safe_read(offset, x, 4); offset += 4;
        safe_read(offset, y, 4); offset += 4;
        safe_read(offset, z, 4); offset += 4;
        V(i, 0) = x;
        V(i, 1) = y;
        V(i, 2) = z;
    }

    Eigen::MatrixXi F(num_faces, 3);
    for (uint32_t i = 0; i < num_faces; ++i) {
        int32_t v0, v1, v2;
        safe_read(offset, v0, 4); offset += 4;
        safe_read(offset, v1, 4); offset += 4;
        safe_read(offset, v2, 4); offset += 4;
        F(i, 0) = v0;
        F(i, 1) = v1;
        F(i, 2) = v2;
    }

    Eigen::MatrixXd P(num_queries, 3);
    for (uint32_t i = 0; i < num_queries; ++i) {
        float x, y, z;
        safe_read(offset, x, 4); offset += 4;
        safe_read(offset, y, 4); offset += 4;
        safe_read(offset, z, 4); offset += 4;
        P(i, 0) = x;
        P(i, 1) = y;
        P(i, 2) = z;
    }

    if (!V.size() || !F.size() || !P.size()) return 0;

    for (int i = 0; i < F.rows(); ++i) {
        for (int j = 0; j < 3; ++j) {
            if (F(i, j) < 0 || F(i, j) >= V.rows()) return 0;
        }
    }

    try {
        FastWindingNumber fwn;
        fwn.build(V, F);
        Eigen::VectorXd W;
        fwn.compute(P, W);
    } catch (...) {
        return 0;
    }

    return 0;
}