include(GoogleTest)
set(PHT_TEST_SOURCES
   bitvector_utils_test.cpp
   farzan_munro_test.cpp
   hypersuccinct_tree_test.cpp
    list_utils_test.cpp
   node_test.cpp
   unordered_tree_test.cpp
    huffman_test.cpp
    caching_test.cpp
)
list(TRANSFORM PHT_TEST_SOURCES PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
add_executable(pht_test)
target_sources(pht_test PRIVATE ${PHT_TEST_SOURCES})
target_link_libraries(pht_test PRIVATE pht_lib gtest gmock gtest_main gmock_main)
gtest_discover_tests(pht_test)
add_custom_command(
    TARGET pht_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:pht_lib>
        $<TARGET_FILE_DIR:pht_test>
)

add_executable(pht_runtime_test)
target_sources(pht_runtime_test PRIVATE runtime_test.cpp)
target_link_libraries(pht_runtime_test PRIVATE pht_lib gtest gmock gtest_main gmock_main)
add_custom_command(
    TARGET pht_runtime_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:pht_lib>
        $<TARGET_FILE_DIR:pht_runtime_test>
)
