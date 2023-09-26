set(TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_interval_map.cpp)

# Test executable
add_executable(runUnitTests ${TEST_SOURCES})

# Adds test
add_test(
        NAME runUnitTests
        COMMAND runUnitTests
)

target_link_libraries(runUnitTests cpp-deepdive)
