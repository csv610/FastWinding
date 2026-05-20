cmake_minimum_required(VERSION 3.20)
project(fastwinding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Compiler Warning Configuration
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic)
endif()

# Compiler Optimizations
if(NOT MSVC)
    add_compile_options($<$<CONFIG:Release>:-O3>)
endif()

# Sanitizers Options
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)

if(ENABLE_ASAN)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
endif()

if(ENABLE_TSAN)
    if(NOT MSVC)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
endif()

# libigl Options
option(LIBIGL_USE_STATIC_LIBRARY "Use libigl as static library" OFF)
option(LIBIGL_WITH_ANTTWEAKBAR       "Use AntTweakBar"    OFF)
option(LIBIGL_WITH_CGAL              "Use CGAL"           OFF)
option(LIBIGL_WITH_COMISO            "Use CoMiso"         OFF)
option(LIBIGL_WITH_CORK              "Use Cork"           OFF)
option(LIBIGL_WITH_EMBREE            "Use Embree"         OFF)
option(LIBIGL_WITH_LIM               "Use LIM"            OFF)
option(LIBIGL_WITH_MATLAB            "Use Matlab"         OFF)
option(LIBIGL_WITH_MOSEK             "Use MOSEK"          OFF)
option(LIBIGL_WITH_OPENGL            "Use OpenGL"         OFF)
option(LIBIGL_WITH_OPENGL_GLFW       "Use GLFW"           OFF)
option(LIBIGL_WITH_OPENGL_GLFW_IMGUI "Use ImGui"          OFF)
option(LIBIGL_WITH_PNG               "Use PNG"            OFF)
option(LIBIGL_WITH_PYTHON            "Use Python"         OFF)
option(LIBIGL_WITH_TETGEN            "Use Tetgen"         OFF)
option(LIBIGL_WITH_TRIANGLE          "Use Triangle"       OFF)
option(LIBIGL_WITH_VIEWER            "Use OpenGL viewer"  OFF)
option(LIBIGL_WITH_XML               "Use XML"            OFF)

find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED)
find_package(LIBIGL REQUIRED QUIET)

# Winding Number Core Source Files
set(WN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/WindingNumber/UT_Array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/WindingNumber/UT_SolidAngle.cpp
)

# Main Executable Target
add_executable(${PROJECT_NAME} main.cpp ${WN_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/WindingNumber
)
target_link_libraries(${PROJECT_NAME} PRIVATE igl::core TBB::tbb)

# Benchmarks
add_executable(benchmark bench/bench_main.cpp ${WN_SOURCES})
target_include_directories(benchmark PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/WindingNumber
)
target_link_libraries(benchmark PRIVATE igl::core TBB::tbb)

add_executable(benchmark_scaling bench/bench_scaling.cpp ${WN_SOURCES})
target_include_directories(benchmark_scaling PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/WindingNumber
)
target_link_libraries(benchmark_scaling PRIVATE igl::core TBB::tbb)

# Fuzz Testing
option(ENABLE_FUZZ "Build fuzz tests (requires libFuzzer)" OFF)

# Unit Tests
enable_testing()

add_executable(unit_tests tests/unit_tests.cpp)
add_test(NAME UnitTests COMMAND unit_tests)

add_test(NAME FastWindingTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} pig-head.ply pig-head-Q.dmat output.dmat
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_FUZZ)
    include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
endif()
