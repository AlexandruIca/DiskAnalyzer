set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(da ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(da PRIVATE project::options project::warnings project::shared)
