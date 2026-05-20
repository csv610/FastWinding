#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <random>

#include "../WindingNumber/UT_SolidAngle.h"
#include "../WindingNumber/UT_FixedVector.h"

#include <igl/read_triangle_mesh.h>
#include <igl/readDMAT.h>
#include <igl/writeDMAT.h>
#include <igl/parallel_for.h>

#include <Eigen/Core>
#include <tbb/tbb.h>

using Vec3 = HDK_Sample::UT_Vector3T<float>;

struct BenchmarkResult {
    std::string name;
    double time_ms;
    size_t items;
};

std::vector<BenchmarkResult> results;

void printResults() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "BENCHMARK RESULTS\n";
    std::cout << std::string(60, '=') << "\n\n";
    std::cout << std::setw(35) << std::left << "Benchmark"
              << std::setw(15) << std::right << "Time (ms)"
              << std::setw(15) << "Items\n";
    std::cout << std::string(60, '-') << "\n";

    for (const auto& r : results) {
        std::cout << std::setw(35) << std::left << r.name
                  << std::setw(15) << std::right << std::fixed << std::setprecision(2) << r.time_ms
                  << std::setw(15) << r.items << "\n";
    }
    std::cout << std::string(60, '=') << "\n";
}

void benchmarkMeshLoading(const std::string& mesh_file) {
    auto start = std::chrono::high_resolution_clock::now();

    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh_file, V, F);

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    results.push_back({"Mesh Loading (" + mesh_file + ")", ms, static_cast<size_t>(V.rows())});
    std::cout << "Loaded mesh: " << V.rows() << " vertices, " << F.rows() << " faces\n";
}

void benchmarkInit(const std::string& mesh_file) {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh_file, V, F);

    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    auto start = std::chrono::high_resolution_clock::now();

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    results.push_back({"Solid Angle Init", ms, static_cast<size_t>(F.rows())});
    std::cout << "Initialized solid angle in " << ms << " ms\n";
}

void benchmarkQuerySerial(const std::string& mesh_file, int num_queries) {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh_file, V, F);

    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);

    std::vector<Vec3> queries(num_queries);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < num_queries; i++) {
        queries[i][0] = dist(rng);
        queries[i][1] = dist(rng);
        queries[i][2] = dist(rng);
    }

    std::vector<float> output(num_queries);
    float accuracy_scale = 2.0f;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_queries; i++) {
        output[i] = solid_angle.computeSolidAngle(queries[i], accuracy_scale);
    }

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    results.push_back({"Query Serial (" + std::to_string(num_queries) + ")", ms, static_cast<size_t>(num_queries)});
    std::cout << "Serial " << num_queries << " queries in " << ms << " ms ("
              << (ms / num_queries * 1000.0) << " us/query)\n";
}

void benchmarkQueryParallel(const std::string& mesh_file, int num_queries, int batch_size) {
    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh_file, V, F);

    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);

    std::vector<Vec3> queries(num_queries);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < num_queries; i++) {
        queries[i][0] = dist(rng);
        queries[i][1] = dist(rng);
        queries[i][2] = dist(rng);
    }

    Eigen::VectorXf output(num_queries);
    float accuracy_scale = 2.0f;

    auto start = std::chrono::high_resolution_clock::now();

    igl::parallel_for(num_queries, [&](int i) {
        output(i) = solid_angle.computeSolidAngle(queries[i], accuracy_scale);
    }, batch_size);

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    results.push_back({"Query Parallel (" + std::to_string(num_queries) + ")", ms, static_cast<size_t>(num_queries)});
    std::cout << "Parallel " << num_queries << " queries (batch " << batch_size << ") in " << ms << " ms\n";
}

void benchmarkVaryingMeshSize() {
    std::cout << "\n--- Varying Mesh Size ---\n";

    std::vector<std::string> meshes = {"pig-head.ply"};
    std::vector<int> query_counts = {1000, 10000};

    for (const auto& mesh : meshes) {
        benchmarkInit(mesh);

        for (int nq : query_counts) {
            benchmarkQuerySerial(mesh, nq);
        }
    }
}

void benchmarkVaryingBatchSize() {
    std::cout << "\n--- Varying Batch Size ---\n";

    const std::string mesh = "pig-head.ply";
    const int num_queries = 50000;
    std::vector<int> batch_sizes = {100, 500, 1000, 5000, 10000};

    for (int bs : batch_sizes) {
        benchmarkQueryParallel(mesh, num_queries, bs);
    }
}

void benchmarkTBBThreads() {
    std::cout << "\n--- TBB Thread Count ---\n";

    std::string mesh = "pig-head.ply";
    int num_queries = 50000;

    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh, V, F);

    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    HDK_Sample::UT_SolidAngle<float, float> solid_angle;
    solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);

    std::vector<Vec3> queries(num_queries);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < num_queries; i++) {
        queries[i][0] = dist(rng);
        queries[i][1] = dist(rng);
        queries[i][2] = dist(rng);
    }

    Eigen::VectorXf output(num_queries);
    float accuracy_scale = 2.0f;

    auto start = std::chrono::high_resolution_clock::now();

    igl::parallel_for(num_queries, [&](int i) {
        output(i) = solid_angle.computeSolidAngle(queries[i], accuracy_scale);
    }, 1000);

    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    results.push_back({"Parallel Query (50k)", ms, static_cast<size_t>(num_queries)});
    std::cout << "Parallel execution: " << ms << " ms\n";
}

void benchmarkAccuracyScales() {
    std::cout << "\n--- Accuracy Scale ---\n";

    std::string mesh = "pig-head.ply";
    int num_queries = 10000;

    std::vector<double> scales = {1.0, 2.0, 4.0, 8.0};

    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    igl::read_triangle_mesh(mesh, V, F);

    std::vector<Vec3> vertices(V.rows());
    for (int i = 0; i < V.rows(); i++) {
        vertices[i][0] = V(i, 0);
        vertices[i][1] = V(i, 1);
        vertices[i][2] = V(i, 2);
    }

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (double scale : scales) {
        HDK_Sample::UT_SolidAngle<float, float> solid_angle;
        solid_angle.init(F.rows(), F.data(), V.rows(), vertices.data(), 2);

        std::vector<Vec3> queries(num_queries);
        for (int i = 0; i < num_queries; i++) {
            queries[i][0] = dist(rng);
            queries[i][1] = dist(rng);
            queries[i][2] = dist(rng);
        }

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_queries; i++) {
            solid_angle.computeSolidAngle(queries[i], scale);
        }

        auto end = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(end - start).count();

        results.push_back({("Accuracy " + std::to_string(scale)), ms, static_cast<size_t>(num_queries)});
        std::cout << "Scale " << scale << ": " << ms << " ms\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout << "======================================\n";
    std::cout << "Fast Winding Number Benchmark Suite\n";
    std::cout << "======================================\n\n";

    benchmarkMeshLoading("pig-head.ply");
    benchmarkInit("pig-head.ply");

    std::cout << "\n--- Serial vs Parallel ---\n";
    benchmarkQuerySerial("pig-head.ply", 1000);
    benchmarkQuerySerial("pig-head.ply", 10000);
    benchmarkQuerySerial("pig-head.ply", 50000);
    benchmarkQueryParallel("pig-head.ply", 1000, 1000);
    benchmarkQueryParallel("pig-head.ply", 10000, 1000);
    benchmarkQueryParallel("pig-head.ply", 50000, 1000);

    benchmarkVaryingBatchSize();
    benchmarkTBBThreads();
    benchmarkAccuracyScales();

    printResults();

    return 0;
}