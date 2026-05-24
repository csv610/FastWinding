#include <cstdint>
#include <cstddef>
#include <vector>
#include "../WindingNumber/UT_SolidAngle.h"

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

    using Vec3 = HDK_Sample::UT_Vector3T<float>;
    std::vector<Vec3> vertices(num_verts);
    for (uint32_t i = 0; i < num_verts; ++i) {
        float x, y, z;
        safe_read(offset, x, 4); offset += 4;
        safe_read(offset, y, 4); offset += 4;
        safe_read(offset, z, 4); offset += 4;
        vertices[i][0] = x;
        vertices[i][1] = y;
        vertices[i][2] = z;
    }

    std::vector<int> indices(num_faces * 3);
    for (uint32_t i = 0; i < num_faces; ++i) {
        int32_t v0, v1, v2;
        safe_read(offset, v0, 4); offset += 4;
        safe_read(offset, v1, 4); offset += 4;
        safe_read(offset, v2, 4); offset += 4;
        if (v0 < 0 || v0 >= static_cast<int32_t>(num_verts)) return 0;
        if (v1 < 0 || v1 >= static_cast<int32_t>(num_verts)) return 0;
        if (v2 < 0 || v2 >= static_cast<int32_t>(num_verts)) return 0;
        indices[i * 3 + 0] = v0;
        indices[i * 3 + 1] = v1;
        indices[i * 3 + 2] = v2;
    }

    std::vector<Vec3> queries(num_queries);
    for (uint32_t i = 0; i < num_queries; ++i) {
        float x, y, z;
        safe_read(offset, x, 4); offset += 4;
        safe_read(offset, y, 4); offset += 4;
        safe_read(offset, z, 4); offset += 4;
        queries[i][0] = x;
        queries[i][1] = y;
        queries[i][2] = z;
    }

    if (num_faces == 0 || num_verts == 0) return 0;

    try {
        HDK_Sample::UT_SolidAngle<float, float> solid_angle;
        solid_angle.init(
            static_cast<int>(num_faces),
            indices.data(),
            static_cast<int>(num_verts),
            vertices.data(),
            2);

        for (uint32_t i = 0; i < num_queries; ++i) {
            solid_angle.computeSolidAngle(queries[i], 2.0f);
        }
    } catch (...) {
        return 0;
    }

    return 0;
}
