project(visp-test-extra)

cmake_minimum_required(VERSION 2.8)

# ctest settings
enable_testing()
include(CTest)

# copy data folder
file(COPY data DESTINATION ${PROJECT_BINARY_DIR})

add_definitions("-DDATA_ROOT_DIR=\"${PROJECT_BINARY_DIR}\"")

add_subdirectory(test/tracking/model-based)
