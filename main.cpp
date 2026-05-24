// Copyright (C) 2018 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#include "WindingNumber/UT_SolidAngle.h"

#include <igl/read_triangle_mesh.h>
#include <igl/parallel_for.h>
#include <igl/readDMAT.h>
#include <igl/writeDMAT.h>

#include <Eigen/Core>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace {

// ANSI Theme Color Escape Codes
const char* RESET       = "\033[0m";
const char* BOLD        = "\033[1m";
const char* RED         = "\033[31m";
const char* GREEN       = "\033[32m";
const char* YELLOW      = "\033[33m";
const char* BLUE        = "\033[34m";
const char* CYAN        = "\033[36m";
const char* BRIGHT_CYAN = "\033[96m";
const char* GRAY        = "\033[90m";

bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool isValidNumber(float x) {
    return std::isfinite(x);
}

void printUsage(const char* programName) {
    std::cerr << BRIGHT_CYAN << BOLD << "\n⚡ FAST WINDING NUMBERS (SideFX Port)" << RESET << "\n";
    std::cerr << GRAY << "=====================================" << RESET << "\n\n";
    std::cerr << BOLD << "USAGE:" << RESET << "\n";
    std::cerr << "  " << programName << " " << CYAN << "input.[mesh|msh|obj|off|ply|stl|wrl]" << RESET << " " << YELLOW << "query.dmat" << RESET << " " << GREEN << "output.dmat" << RESET << "\n\n";
    
    std::cerr << BOLD << "ARGUMENTS:" << RESET << "\n";
    std::cerr << "  " << CYAN << "input.*" << RESET << "     Path to input 3D triangle mesh file\n";
    std::cerr << "  " << YELLOW << "query.dmat" << RESET << "  Path to Nx3 matrix of query points\n";
    std::cerr << "  " << GREEN << "output.dmat" << RESET << " Path to output file for winding numbers\n\n";
    
    std::cerr << BOLD << "SUPPORTED FORMATS:" << RESET << "\n";
    std::cerr << "  Mesh:  PLY, OBJ, OFF, STL, WRL, MSH\n";
    std::cerr << "  Query: DMAT (libigl matrix format)\n\n";
    
    std::cerr << BOLD << "EXIT CODES:" << RESET << "\n";
    std::cerr << "  " << GREEN << "0" << RESET << " - Success\n";
    std::cerr << "  " << RED << "1" << RESET << " - Invalid arguments\n";
    std::cerr << "  " << RED << "2" << RESET << " - File not found, cannot be read, or unsupported format\n";
    std::cerr << "  " << RED << "3" << RESET << " - Invalid mesh data\n";
    std::cerr << "  " << RED << "4" << RESET << " - Invalid query points (NaN/Inf)\n";
    std::cerr << "  " << RED << "5" << RESET << " - Write error\n\n";
}

int validateMesh(const Eigen::MatrixXf& V, const Eigen::MatrixXi& F) {
    // Check vertex count
    if (V.rows() == 0) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Mesh has no vertices" << RESET << "\n";
        return 3;
    }
    
    // Check face count  
    if (F.rows() == 0) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Mesh has no faces" << RESET << "\n";
        return 3;
    }
    
    // Check vertex dimensions
    if (V.cols() != 3) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Vertices must be 3D (expected 3 columns, got " << V.cols() << ")" << RESET << "\n";
        return 3;
    }
    
    // Check face dimensions
    if (F.cols() != 3) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Faces must be triangles (expected 3 columns, got " << F.cols() << ")" << RESET << "\n";
        return 3;
    }
    
    // Check for NaN or Inf in vertices
    for (int i = 0; i < V.rows(); ++i) {
        for (int j = 0; j < 3; ++j) {
            if (!isValidNumber(V(i, j))) {
                std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Vertex " << i << " has invalid coordinate: " << V(i, j) << RESET << "\n";
                return 3;
            }
        }
    }
    
    // Check face indices are valid
    for (int i = 0; i < F.rows(); ++i) {
        for (int j = 0; j < 3; ++j) {
            if (F(i, j) < 0 || F(i, j) >= V.rows()) {
                std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Face " << i << " references invalid vertex " << F(i, j) 
                          << " (valid range: 0-" << V.rows() - 1 << ")" << RESET << "\n";
                return 3;
            }
        }
    }
    
    return 0;
}

int validateQueryPoints(const Eigen::MatrixXf& P) {
    if (P.rows() == 0) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "No query points provided" << RESET << "\n";
        return 4;
    }
    
    if (P.cols() != 3) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Query points must be 3D (expected 3 columns, got " << P.cols() << ")" << RESET << "\n";
        return 4;
    }
    
    // Check for NaN or Inf in query points
    int invalidCount = 0;
    for (int i = 0; i < P.rows(); ++i) {
        if (!isValidNumber(P(i, 0)) || !isValidNumber(P(i, 1)) || !isValidNumber(P(i, 2))) {
            invalidCount++;
        }
    }
    
    if (invalidCount > 0) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << invalidCount << " query points contain NaN or Inf values" << RESET << "\n";
        return 4;
    }
    
    return 0;
}

}

int main(int argc, char * argv[])
{
    // Validate argument count
    if (argc != 4)
    {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char* meshFile = argv[1];
    const char* queryFile = argv[2];
    const char* outputFile = argv[3];
    
    // Print Beautiful Header Banner
    std::cout << "\n"
              << BRIGHT_CYAN << BOLD << " ⚡ FAST WINDING NUMBERS (SideFX Port)" << RESET << "\n"
              << GRAY << "  =====================================" << RESET << "\n\n";
    
    // Validate input files exist
    if (!fileExists(meshFile)) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Mesh file not found: " << meshFile << RESET << "\n\n";
        return 2;
    }
    
    if (!fileExists(queryFile)) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Query file not found: " << queryFile << RESET << "\n\n";
        return 2;
    }
    
    // Read mesh
    Eigen::MatrixXf V;
    Eigen::Matrix<int,Eigen::Dynamic,3,Eigen::RowMajor> F;
    
    std::cout << BLUE << BOLD << " [1/5]" << RESET << " 📦 Loading mesh: " << CYAN << meshFile << RESET << "...\n";
    bool meshLoaded = igl::read_triangle_mesh(meshFile, V, F);
    
    if (!meshLoaded) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Failed to read mesh file: " << meshFile << RESET << "\n";
        std::cerr << RED << "            Check that the file format is supported (PLY, OBJ, OFF, STL, WRL, MSH)" << RESET << "\n\n";
        return 2;
    }
    
    std::cout << GRAY << "        ● Loaded " << BOLD << V.rows() << RESET << GRAY << " vertices, " << BOLD << F.rows() << RESET << GRAY << " faces" << RESET << "\n\n";
    
    // Validate mesh data
    int validationResult = validateMesh(V, F);
    if (validationResult != 0) {
        return validationResult;
    }
    
    // Read query points
    Eigen::MatrixXf P;
    
    std::cout << BLUE << BOLD << " [2/5]" << RESET << " 📍 Loading query points: " << CYAN << queryFile << RESET << "...\n";
    bool queryLoaded = igl::readDMAT(queryFile, P);
    
    if (!queryLoaded) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Failed to read query file: " << queryFile << RESET << "\n";
        std::cerr << RED << "            Check that the file is in DMAT format" << RESET << "\n\n";
        return 2;
    }
    
    std::cout << GRAY << "        ● Loaded " << BOLD << P.rows() << RESET << GRAY << " query points" << RESET << "\n\n";
    
    // Validate query points
    validationResult = validateQueryPoints(P);
    if (validationResult != 0) {
        return validationResult;
    }
    
    // Initialize solid angle calculator
    std::cout << BLUE << BOLD << " [3/5]" << RESET << " ⚙️  Initializing solid angle calculator...\n\n";
    
    std::vector<HDK_Sample::UT_Vector3T<float>> U(V.rows());
    for (int i = 0; i < V.rows(); ++i) {
        U[i][0] = V(i, 0);
        U[i][1] = V(i, 1);
        U[i][2] = V(i, 2);
    }
    
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    int order = 2;
    double accuracy_scale = 2.0;
    
    solid_angle.init(
        F.rows(),
        F.data(),
        V.rows(),
        U.data(),
        order);
    
    std::cout << BLUE << BOLD << " [4/5]" << RESET << " 🧮 Computing winding numbers in parallel...\n\n";
    
    // Compute winding numbers in parallel
    Eigen::VectorXf W(P.rows());
    
    igl::parallel_for(P.rows(), [&](int p) {
        HDK_Sample::UT_Vector3T<float> Pp;
        Pp[0] = P(p, 0);
        Pp[1] = P(p, 1);
        Pp[2] = P(p, 2);
        
        float solid = solid_angle.computeSolidAngle(Pp, accuracy_scale);
        W(p) = solid / (4.0 * M_PI);
    }, 1000);
    
    // Validate output values
    int nanCount = 0;
    for (int i = 0; i < W.rows(); ++i) {
        if (!isValidNumber(W(i))) {
            nanCount++;
            W(i) = 0.0f;
        }
    }
    
    if (nanCount > 0) {
        std::cerr << YELLOW << BOLD << "  ⚠️  WARNING: " << RESET << YELLOW << nanCount << " output values are NaN, set to 0" << RESET << "\n\n";
    }
    
    // Write output
    std::cout << BLUE << BOLD << " [5/5]" << RESET << " 💾 Writing output to: " << CYAN << outputFile << RESET << "...\n\n";
    
    bool writeSuccess = igl::writeDMAT(outputFile, W, false);
    
    if (!writeSuccess) {
        // Check if file was created anyway
        std::ifstream test(outputFile);
        if (!test.good()) {
            std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Failed to write output file: " << outputFile << RESET << "\n\n";
            return 5;
        }
    }
    
    // Verify output
    std::ifstream verify(outputFile);
    if (!verify.good()) {
        std::cerr << RED << BOLD << "  ❌ ERROR: " << RESET << RED << "Output file not created" << RESET << "\n\n";
        return 5;
    }
    
    // Compute statistics
    int inside = 0, outside = 0, onSurface = 0;
    for (int i = 0; i < W.rows(); ++i) {
        if (W(i) > 0.99f) inside++;
        else if (W(i) < 0.01f) outside++;
        else onSurface++;
    }
    
    // Render Results Dashboard Panel
    char insideStr[64];
    std::snprintf(insideStr, sizeof(insideStr), "%d (%.2f%%)", inside, (100.0 * inside / W.rows()));
    char outsideStr[64];
    std::snprintf(outsideStr, sizeof(outsideStr), "%d (%.2f%%)", outside, (100.0 * outside / W.rows()));
    char onSurfaceStr[64];
    std::snprintf(onSurfaceStr, sizeof(onSurfaceStr), "%d (%.2f%%)", onSurface, (100.0 * onSurface / W.rows()));

    std::cout << BRIGHT_CYAN << BOLD << " ┌────────────────────────────────────────────────────────┐" << RESET << "\n";
    std::cout << BRIGHT_CYAN << BOLD << " │                       " << RESET << BOLD << "RESULTS" << BRIGHT_CYAN << BOLD << "                          │" << RESET << "\n";
    std::cout << BRIGHT_CYAN << BOLD << " ├────────────────────────────────────────────────────────┤" << RESET << "\n";
    std::printf(" %s│%s  Total query points: %-34d%s│%s\n", BRIGHT_CYAN, RESET, (int)W.rows(), BRIGHT_CYAN, RESET);
    std::printf(" %s│%s  %s●%s Inside mesh:       %-33s%s│%s\n", BRIGHT_CYAN, RESET, GREEN, RESET, insideStr, BRIGHT_CYAN, RESET);
    std::printf(" %s│%s  %s●%s Outside mesh:      %-33s%s│%s\n", BRIGHT_CYAN, RESET, RED, RESET, outsideStr, BRIGHT_CYAN, RESET);
    std::printf(" %s│%s  %s●%s On surface:        %-33s%s│%s\n", BRIGHT_CYAN, RESET, YELLOW, RESET, onSurfaceStr, BRIGHT_CYAN, RESET);
    std::cout << BRIGHT_CYAN << BOLD << " ├────────────────────────────────────────────────────────┤" << RESET << "\n";
    std::printf(" %s│%s  Output written to:  %-34s%s│%s\n", BRIGHT_CYAN, RESET, outputFile, BRIGHT_CYAN, RESET);
    std::cout << BRIGHT_CYAN << BOLD << " └────────────────────────────────────────────────────────┘" << RESET << "\n\n";
    
    return EXIT_SUCCESS;
}
