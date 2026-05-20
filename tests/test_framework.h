#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <functional>

namespace TestFramework {

class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }

    void addTest(const std::string& suite, const std::string& name, std::function<bool()> func) {
        tests.push_back({suite, name, func});
    }

    int runAll() {
        int passed = 0;
        int failed = 0;
        std::string currentSuite;

        for (auto& test : tests) {
            if (test.suite != currentSuite) {
                currentSuite = test.suite;
                std::cout << "\n=== " << currentSuite << " ===\n";
            }

            try {
                bool result = test.func();
                if (result) {
                    std::cout << "[PASS] " << test.name << "\n";
                    passed++;
                } else {
                    std::cout << "[FAIL] " << test.name << "\n";
                    failed++;
                }
            } catch (const std::exception& e) {
                std::cout << "[FAIL] " << test.name << " - Exception: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << "[FAIL] " << test.name << " - Unknown exception\n";
                failed++;
            }
        }

        std::cout << "\n=== Summary ===\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "Total:  " << (passed + failed) << "\n";

        return failed;
    }

private:
    struct Test {
        std::string suite;
        std::string name;
        std::function<bool()> func;
    };
    std::vector<Test> tests;
};

#define TEST(suite, name) \
    bool test_##suite##_##name(); \
    namespace { \
        struct TestReg_##suite##_##name { \
            TestReg_##suite##_##name() { \
                TestFramework::TestRunner::instance().addTest(#suite, #name, test_##suite##_##name); \
            } \
        } reg_##suite##_##name; \
    } \
    bool test_suite##_##name()

#define ASSERT_TRUE(x) \
    do { \
        if (!(x)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #x; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_FALSE(x) ASSERT_TRUE(!(x))

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #a << " == " << #b \
                << " (got " << (a) << " vs " << (b) << ")"; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_NE(a, b) \
    do { \
        if ((a) == (b)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #a << " != " << #b; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_FLOAT_EQ(a, b, eps) \
    do { \
        if (std::fabs((a) - (b)) > (eps)) { \
            std::ostringstream oss; \
            oss << "Assertion failed: " << #a << " == " << #b \
                << " (got " << (a) << " vs " << (b) << ")"; \
            throw std::runtime_error(oss.str()); \
        } \
    } while(0)

#define ASSERT_NEAR(a, b, tolerance) ASSERT_FLOAT_EQ(a, b, tolerance)