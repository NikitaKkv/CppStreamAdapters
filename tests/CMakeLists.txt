include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    FIND_PACKAGE_ARGS NAMES GTest
)

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    processing-lib-tests
    drop_nullopt_ut.cpp
    filter_ut.cpp
    flow_convertions_ut.cpp
    join_ut.cpp
    aggregate_by_key_ut.cpp
    split_expected_ut.cpp
    split_ut.cpp
    transform_ut.cpp
    write_ut.cpp
    my_tests.cpp
)

target_link_libraries(
    processing-lib-tests
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(processing-lib-tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(processing-lib-tests)
