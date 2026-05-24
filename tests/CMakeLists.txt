# Fuzz Testing Configuration
# Build with: cmake -DENABLE_FUZZ=ON ..
# Run: ./fuzz_winding -artifact_prefix=fuzz- -max_total_time=60s fuzz_corpus/

if(ENABLE_FUZZ)
    find_package(FuzzingEngine)

    add_executable(fuzz_winding
        fuzz_winding.cpp
        ../WindingNumber/UT_SolidAngle.cpp
        ../WindingNumber/UT_Array.cpp
    )

    target_link_libraries(fuzz_winding
        Eigen3::Eigen
        TBB::tbb
        FuzzingEngine
    )

    target_compile_options(fuzz_winding PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
    )

    target_link_options(fuzz_winding PRIVATE
        -fsanitize=fuzzer,address,undefined
    )

    add_custom_target(fuzz
        COMMAND ./fuzz_winding -max_total_time=60s ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_corpus/
        DEPENDS fuzz_winding
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running fuzz tests for 60 seconds"
    )

    message(STATUS "Fuzz testing enabled. Run 'make fuzz' to execute.")
endif()