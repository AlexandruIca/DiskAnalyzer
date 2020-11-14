set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(shared STATIC ${CMAKE_CURRENT_SOURCE_DIR}/socket.cpp)
add_library(project::shared ALIAS shared)

target_include_directories(shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shared PRIVATE project::options project::warnings)
