#pragma once
#include <vector>
#include <array>
#include <concepts>

/**
 * @namespace fastwinding
 * @brief Namespace for the Fast Winding Number library.
 */
namespace fastwinding {

/**
 * @struct MeshObject
 * @brief A lightweight structure representing a triangular mesh.
 * 
 * This structure holds the vertex positions and face indices.
 * It is designed to be easily populated from external geometry libraries.
 */
struct MeshObject {
    /**
     * @brief Vertices of the mesh.
     * Each vertex is a 3D point represented by an array of 3 floats.
     */
    std::vector<std::array<float, 3>> vertices;
    
    /**
     * @brief Faces of the mesh.
     * Each face is a triangle represented by 3 indices into the vertex array.
     */
    std::vector<std::array<int, 3>> faces;
    
    /**
     * @brief Verifies if the mesh has been initialized with data.
     * @return true if both vertices and faces are non-empty.
     */
    [[nodiscard]] bool isValid() const noexcept {
        return !vertices.empty() && !faces.empty();
    }
};

} // namespace fastwinding
