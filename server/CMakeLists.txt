set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(da_server ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp)
target_link_libraries(da_server PRIVATE project::options project::warnings project::shared)
