add_library(utils utils.cpp)
add_library(steiner_strategies steiner_strategies.cpp)
add_library(json_loader JosnLoader.cpp)
add_library(json_exporter JsonExporter.cpp)
add_library(ant_colony_structures AntColonyStructures.cpp)

# Include the current directory for headers
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ant_colony_structures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(steiner_strategies PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(json_loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(json_exporter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
