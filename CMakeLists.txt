cmake_minimum_required(VERSION 3.16.3)
project(behavior_flow)

set(CMAKE_CXX_STANDARD 20)
set(BF_LIBRARY ${PROJECT_NAME})

add_library(${BF_LIBRARY}
    src/node_graph.cpp
    # src/graph_file_parser_interface.cpp
    # src/graph_file_parser_json.cpp
    # src/behavior_flow_engine.cpp
    src/behavior_flow_node.cpp
    src/node_factory.cpp
    src/node_registrar.cpp
    src/node_instance_provider.cpp
)
target_include_directories(${BF_LIBRARY} PUBLIC include)

# Option to build unit tests, default ON
option(BUILD_TESTING "Build the unit tests" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()