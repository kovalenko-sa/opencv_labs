cmake_minimum_required(VERSION 3.0.0)
project(opencv_study VERSION 0.1.0)

include(CTest)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})
enable_testing()

add_executable(main main.cpp)
add_executable(tank tank.cpp)
add_executable(blending blending.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(tank ${OpenCV_LIBS})
target_link_libraries(blending ${OpenCV_LIBS})
