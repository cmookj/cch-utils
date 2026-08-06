set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(cch-utils-test
    test.cpp
)

target_compile_options(cch-utils-test PRIVATE
    $<$<CONFIG:Debug>:-g -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -Wall -Wextra -Wpedantic>
)

target_link_libraries(cch-utils-test PRIVATE
  cch-utils
  GTest::gtest_main
)

target_include_directories(cch-utils-test PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(cch-utils-test)
