set(TEST_SOURCES
    test_node_graph.cpp
    # test_graph_file_parser_json.cpp
    # test_behavior_flow_engine.cpp
    # test_node_factory.cpp
    # src/nodes_for_testing.cpp
)

include_directories(include)

find_package(GTest REQUIRED)
enable_testing()

foreach(test_file IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name}
        ${BF_LIBRARY} # Consider only adding deps required for each test
        Threads::Threads
        GTest::gtest
        GTest::gtest_main
        )

    add_test(NAME ${test_name} COMMAND ${test_name}) 
endforeach()