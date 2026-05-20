#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <random>
#include <cmath>

#include "../WindingNumber/UT_SolidAngle.h"
#include "../WindingNumber/UT_FixedVector.h"

#include <Eigen/Core>

using Vec3 = HDK_Sample::UT_Vector3T<float>;

Eigen::MatrixXf generateSphere(int resolution) {
    int n_vert = (resolution + 1) * (resolution + 1);
    Eigen::MatrixXf V(n_vert, 3);
    Eigen::MatrixXi F(resolution * resolution * 2, 3);

    float radius = 1.0f;

    int idx = 0;
    for (int lat = 0; lat <= resolution; lat++) {
        float theta = M_PI * lat / resolution;
        for (int lon = 0; lon <= resolution; lon++) {
            float phi = 2.0f * M_PI * lon / resolution;
            V(idx, 0) = radius * sinf(theta) * cosf(phi);
            V(idx, 1) = radius * sinf(theta) * sinf(phi);
            V(idx, 2) = radius * cosf(theta);
            idx++;
        }
    }

    idx = 0;
    for (int lat = 0; lat < resolution; lat++) {
        for (int lon = 0; lon < resolution; lon++) {
            int first = lat * (resolution + 1) + lon;
            int second = first + resolution + 1;
            F(idx, 0) = first;
            F(idx, 1) = second;
            F(idx, 2) = first + 1;
            idx++;
            F(idx, 0) = second;
            F(idx, 1) = second + 1;
            F(idx, 2) = first + 1;
            idx++;
        }
    }

    return V;
}

void runBenchmark(const std::string& name, const Eigen::MatrixXf& V, const Eigen::MatrixXi& F, int num_queries) {
    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    auto start_init = std::chrono::high_resolution_clock::now();
    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);
    auto end_init = std::chrono::high_resolution_clock::now();
    double init_ms = std::chrono::duration<double, std::milli>(end_init - start_init).count();

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-2.0f, 2.0f);

    std::vector<Vec3> queries(num_queries);
    for (int i = 0; i < num_queries; i++) {
        queries[i][0] = dist(rng);
        queries[i][1] = dist(rng);
        queries[i][2] = dist(rng);
    }

    Eigen::VectorXf output(num_queries);
    float accuracy_scale = 2.0f;

    auto start_query = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_queries; i++) {
        output(i) = solid_angle.computeSolidAngle(queries[i], accuracy_scale);
    }
    auto end_query = std::chrono::high_resolution_clock::now();
    double query_ms = std::chrono::duration<double, std::milli>(end_query - start_query).count();

    double us_per_query = (query_ms * 1000.0) / num_queries;

    std::cout << std::setw(20) << name << " | " << std::setw(10) << V.rows() 
              << " | " << std::setw(10) << F.rows() << " | " << std::setw(12) << std::fixed << std::setprecision(2) << init_ms 
              << " | " << std::setw(12) << query_ms << " | " << std::setw(10) << us_per_query << "\n";
}

int main() {
    std::cout << "==================================================================================================\n";
    std::cout << "MESH SIZE SCALING ANALYSIS - INDUSTRIAL WORKLOADS\n";
    std::cout << "==================================================================================================\n";
    std::cout << std::setw(20) << "Mesh" << " | " << std::setw(10) << "Vertices" 
              << " | " << std::setw(10) << "Faces" << " | " << std::setw(12) << "Init (ms)" 
              << " | " << std::setw(12) << "Query (ms)" << " | " << std::setw(10) << "us/query" << "\n";
    std::cout << std::string(95, '-') << "\n";

    const int num_queries = 10000;

    std::vector<int> sizes = {256, 512, 1024, 2048};
    for (int s : sizes) {
        int faces = s * s * 2;
        std::cout << "Generating sphere with " << faces << " faces...\n";
        
        Eigen::MatrixXf V = generateSphere(s);
        Eigen::MatrixXi F(s * s * 2, 3);
        int idx = 0;
        for (int lat = 0; lat < s; lat++) {
            for (int lon = 0; lon < s; lon++) {
                int first = lat * (s + 1) + lon;
                int second = first + s + 1;
                F(idx, 0) = first; F(idx, 1) = second; F(idx, 2) = first + 1; idx++;
                F(idx, 0) = second; F(idx, 1) = second + 1; F(idx, 2) = first + 1; idx++;
            }
        }
        
        std::string name = "Sphere_" + std::to_string(s);
        if (faces >= 1000000) {
            name += " (" + std::to_string(faces/1000000.0) + "M)";
        }
        
        runBenchmark(name, V, F, num_queries);
    }

    return 0;
}