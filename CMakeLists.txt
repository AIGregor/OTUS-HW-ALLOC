cmake_minimum_required(VERSION 3.2)

project(hw-alloc VERSION 0.0.1)

add_executable(hw-alloc_cli main.cpp)

target_link_libraries(hw-alloc_cli hw-alloc)

install(TARGETS hw-alloc_cli RUNTIME DESTINATION bin)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")