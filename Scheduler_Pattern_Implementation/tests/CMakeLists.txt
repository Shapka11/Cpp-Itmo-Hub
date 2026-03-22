include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    scheduler-tests
    test.cpp
    
)

target_link_libraries(
    scheduler-tests
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(scheduler-tests PUBLIC ${PROJECT_SOURCE_DIR})

include(GoogleTest)

gtest_discover_tests(scheduler-tests)
