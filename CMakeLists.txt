cmake_minimum_required(VERSION 2.8)

project(ChatGLM)

file(GLOB hdrs "include/*.h" "include/*.hpp")
file(GLOB srcs "src/*.cpp")
file(GLOB test "test/*.cpp" "test/*.h")


source_group("include" FILES ${hdrs})
source_group("src" FILES ${srcs})
source_group("test" FILES ${test})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(${PROJECT_NAME} ${hdrs} ${srcs} ${test}) 
target_link_libraries(${PROJECT_NAME})