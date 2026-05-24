#pragma once
#include <vector>
#include <array>

namespace fastwinding {

struct MeshObject {
    // Vertices as standard STL vector of 3-element float arrays
    std::vector<std::array<float, 3>> vertices;
    
    // Faces as standard STL vector of 3-element int arrays (triangle indices)
    std::vector<std::array<int, 3>> faces;
    
    // Helper to verify mesh initialization
    bool isValid() const {
        return !vertices.empty() && !faces.empty();
    }
};

} // namespace fastwinding
