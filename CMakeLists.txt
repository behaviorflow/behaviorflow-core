cmake_minimum_required(VERSION 3.16.3)
project(behavior_flow)

set(CMAKE_CXX_STANDARD 20)
set(BF_LIBRARY ${PROJECT_NAME})

add_library(${BF_LIBRARY}
    src/node_graph.cpp
    src/json_graph_parser.cpp
    src/behavior_flow_engine.cpp
    src/node_factory.cpp
    src/node_registry.cpp
    src/node_instance_provider.cpp
    src/standard_node_library.cpp
)
target_include_directories(${BF_LIBRARY} PUBLIC include)

option(BFLOW_USE_SYSTEM_JSON "Use system nlohmann/json instead of vendored" OFF)
if(BFLOW_USE_SYSTEM_JSON)
    find_package(nlohmann_json 3.12.0 REQUIRED)
    target_link_libraries(${BF_LIBRARY} PRIVATE nlohmann_json::nlohmann_json)
else()
    target_include_directories(${BF_LIBRARY} 
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/json
    )
endif()

# Option to build unit tests, default ON
option(BFLOW_BUILD_TESTING "Build the unit tests" ON)

if(BFLOW_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()