set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
  da_server_deps STATIC ${CMAKE_CURRENT_SOURCE_DIR}/filesystem.cpp ${CMAKE_CURRENT_SOURCE_DIR}/task_manager.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/thread_pool.cpp ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp)
add_library(project::server_utils ALIAS da_server_deps)
target_link_libraries(da_server_deps PUBLIC project::options project::warnings)

add_executable(da_server ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp)
target_link_libraries(da_server PRIVATE project::options project::warnings project::shared project::server_utils)
