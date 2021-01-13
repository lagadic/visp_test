project(visp-test-extra)

cmake_minimum_required(VERSION 2.8)

# By default set release configuration
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None Debug Release" FORCE)
endif()

# ctest settings
enable_testing()
include(CTest)

# copy data folder
file(COPY data DESTINATION ${PROJECT_BINARY_DIR})

add_definitions("-DDATA_ROOT_DIR=\"${PROJECT_BINARY_DIR}\"")

add_subdirectory(test/tracking/model-based)
