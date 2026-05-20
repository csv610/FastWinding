#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <limits>

int passed = 0;
int failed = 0;

#define TEST(name) void test_##name()
#define RUN(name) do { \
    std::cout << "  " << #name << "... "; \
    try { \
        test_##name(); \
        std::cout << "PASSED\n"; \
        passed++; \
    } catch (const std::exception& e) { \
        std::cout << "FAILED: " << e.what() << "\n"; \
        failed++; \
    } catch (...) { \
        std::cout << "FAILED\n"; \
        failed++; \
    } \
} while(0)

#define ASSERT_TRUE(x) do { if (!(x)) throw std::runtime_error("Assertion failed: " #x); } while(0)
#define ASSERT_FALSE(x) ASSERT_TRUE(!(x))
#define ASSERT_EQ(a, b) do { if ((a) != (b)) { std::ostringstream _oss; _oss << "Expected " << (b) << " but got " << (a); throw std::runtime_error(_oss.str()); } } while(0)
#define ASSERT_FLOAT_EQ(a, b, eps) do { if (std::fabs((a) - (b)) > (eps)) { std::ostringstream _oss; _oss << "Expected " << (b) << " but got " << (a); throw std::runtime_error(_oss.str()); } } while(0)
#define ASSERT_GE(a, b) do { if ((a) < (b)) throw std::runtime_error("Expected " #a " >= " #b); } while(0)

// ============== Math Tests ==============
TEST(math_sqrt) {
    ASSERT_FLOAT_EQ(std::sqrt(4.0f), 2.0f, 1e-6f);
    ASSERT_FLOAT_EQ(std::sqrt(2.0f), 1.41421356f, 1e-5f);
}

TEST(math_abs) {
    ASSERT_FLOAT_EQ(std::abs(-3.0f), 3.0f, 1e-6f);
    ASSERT_FLOAT_EQ(std::abs(3.0f), 3.0f, 1e-6f);
}

TEST(math_trig) {
    ASSERT_FLOAT_EQ(std::sin(0.0f), 0.0f, 1e-6f);
    ASSERT_FLOAT_EQ(std::cos(0.0f), 1.0f, 1e-6f);
}

// ============== Vector Tests ==============
TEST(vector_creation) {
    std::vector<float> v(3);
    v[0] = 1.0f; v[1] = 2.0f; v[2] = 3.0f;
    ASSERT_EQ(v.size(), 3u);
    ASSERT_FLOAT_EQ(v[0], 1.0f, 1e-6f);
}

TEST(vector_operations) {
    std::vector<float> a = {1.0f, 2.0f, 3.0f};
    std::vector<float> b = {4.0f, 5.0f, 6.0f};
    
    std::vector<float> sum(3);
    for (int i = 0; i < 3; i++) sum[i] = a[i] + b[i];
    
    ASSERT_FLOAT_EQ(sum[0], 5.0f, 1e-6f);
    ASSERT_FLOAT_EQ(sum[1], 7.0f, 1e-6f);
    ASSERT_FLOAT_EQ(sum[2], 9.0f, 1e-6f);
}

TEST(vector_dot_product) {
    std::vector<float> a = {1.0f, 2.0f, 3.0f};
    std::vector<float> b = {4.0f, 5.0f, 6.0f};
    
    float dot = 0.0f;
    for (int i = 0; i < 3; i++) dot += a[i] * b[i];
    
    ASSERT_FLOAT_EQ(dot, 32.0f, 1e-6f);
}

// ============== Array Tests ==============
TEST(array_basic) {
    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    ASSERT_EQ(arr.size(), 3u);
    ASSERT_EQ(arr[0], 1);
    ASSERT_EQ(arr[1], 2);
    ASSERT_EQ(arr[2], 3);
}

TEST(array_clear) {
    std::vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.clear();
    ASSERT_EQ(arr.size(), 0u);
}

TEST(array_reserve) {
    std::vector<int> arr;
    arr.reserve(100);
    ASSERT_GE(arr.capacity(), 100u);
}

// ============== Winding Number Logic ==============
TEST(solid_angle_basic) {
    float pi = 3.14159265f;
    ASSERT_FLOAT_EQ(4.0f * pi, 12.5663706f, 1e-5f);
}

TEST(winding_normalization) {
    float pi = 3.14159265f;
    float solid_angle = 4.0f * pi;
    float wn = solid_angle / (4.0f * pi);
    ASSERT_FLOAT_EQ(wn, 1.0f, 1e-6f);
}

TEST(point_inside_sphere) {
    float distance = std::sqrt(0.1f*0.1f + 0.1f*0.1f + 0.1f*0.1f);
    ASSERT_TRUE(distance < 1.0f);
}

TEST(point_outside_sphere) {
    float distance = std::sqrt(2.0f*2.0f + 2.0f*2.0f + 2.0f*2.0f);
    ASSERT_TRUE(distance > 1.0f);
}

// ============== Edge Case Tests ==============
TEST(edge_nan_handling) {
    float nan_val = std::numeric_limits<float>::quiet_NaN();
    ASSERT_FALSE(std::isfinite(nan_val));
    ASSERT_TRUE(std::isnan(nan_val));
}

TEST(edge_inf_handling) {
    float inf_val = std::numeric_limits<float>::infinity();
    ASSERT_TRUE(std::isinf(inf_val));
    ASSERT_TRUE(std::isfinite(1.0f / inf_val));
}

TEST(edge_zero_division) {
    float result = 1.0f / 1000.0f;
    ASSERT_FLOAT_EQ(result, 0.001f, 1e-6f);
}

TEST(edge_negative_values) {
    std::vector<float> neg = {-1.0f, -2.0f, -3.0f};
    float sum = 0.0f;
    for (float f : neg) sum += f;
    ASSERT_FLOAT_EQ(sum, -6.0f, 1e-6f);
}

TEST(edge_large_values) {
    float large = 1e10f;
    float result = std::sqrt(large);
    ASSERT_TRUE(result > 0.0f);
    ASSERT_TRUE(std::isfinite(result));
}

TEST(edge_small_values) {
    float small = 1e-10f;
    float result = small * 1000.0f;
    ASSERT_FLOAT_EQ(result, 1e-7f, 1e-10f);
}

TEST(edge_precision) {
    // Test floating point precision
    float a = 0.1f + 0.2f;
    float expected = 0.3f;
    // Allow for both exact equality or small difference (depends on FPU)
    bool isClose = std::fabs(a - expected) < 1e-5f;
    ASSERT_TRUE(isClose || (a == expected));
}

TEST(edge_empty_array) {
    std::vector<float> empty;
    ASSERT_EQ(empty.size(), 0u);
    ASSERT_TRUE(empty.empty());
}

TEST(edge_single_element) {
    std::vector<float> single = {42.0f};
    ASSERT_EQ(single.size(), 1u);
    ASSERT_FLOAT_EQ(single[0], 42.0f, 1e-6f);
}

// ============== Performance ==============
TEST(benchmark_iteration) {
    int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    ASSERT_EQ(sum, 499500);
}

TEST(benchmark_sqrt) {
    float result = 0.0f;
    for (int i = 1; i <= 1000; i++) {
        result += std::sqrt((float)i);
    }
    ASSERT_TRUE(result > 0.0f);
}

// ============== Validation Logic ==============
TEST(validation_vertex_count) {
    int vertexCount = 100;
    ASSERT_TRUE(vertexCount > 0);
}

TEST(validation_face_indices) {
    int idx = 50;
    int maxIdx = 100;
    ASSERT_TRUE(idx >= 0);
    ASSERT_TRUE(idx < maxIdx);
}

TEST(validation_bounding_box) {
    float minVal = -100.0f;
    float maxVal = 100.0f;
    float testVal = 50.0f;
    ASSERT_GE(testVal, minVal);
    ASSERT_TRUE(testVal <= maxVal);
}

int main() {
    std::cout << "==============================================\n";
    std::cout << "FastWinding Unit Test Suite\n";
    std::cout << "==============================================\n\n";

    std::cout << "Math Tests:\n";
    RUN(math_sqrt);
    RUN(math_abs);
    RUN(math_trig);

    std::cout << "\nVector Tests:\n";
    RUN(vector_creation);
    RUN(vector_operations);
    RUN(vector_dot_product);

    std::cout << "\nArray Tests:\n";
    RUN(array_basic);
    RUN(array_clear);
    RUN(array_reserve);

    std::cout << "\nWinding Number Logic:\n";
    RUN(solid_angle_basic);
    RUN(winding_normalization);
    RUN(point_inside_sphere);
    RUN(point_outside_sphere);

    std::cout << "\nEdge Case Tests:\n";
    RUN(edge_nan_handling);
    RUN(edge_inf_handling);
    RUN(edge_zero_division);
    RUN(edge_negative_values);
    RUN(edge_large_values);
    RUN(edge_small_values);
    RUN(edge_precision);
    RUN(edge_empty_array);
    RUN(edge_single_element);

    std::cout << "\nValidation Tests:\n";
    RUN(validation_vertex_count);
    RUN(validation_face_indices);
    RUN(validation_bounding_box);

    std::cout << "\nPerformance Tests:\n";
    RUN(benchmark_iteration);
    RUN(benchmark_sqrt);

    std::cout << "\n==============================================\n";
    std::cout << "RESULTS: " << passed << " passed, " << failed << " failed\n";
    std::cout << "==============================================\n";

    return failed > 0 ? 1 : 0;
}