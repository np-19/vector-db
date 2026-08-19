add_executable(foundation_test foundation_test.cpp)
target_link_libraries(foundation_test PRIVATE MiniVectorDB::minivectordb GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(foundation_test)
