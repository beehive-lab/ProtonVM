cmake_minimum_required(VERSION 3.19)
project(protonVM)

# set the C++14 standard
set(CMAKE_CXX_STANDARD 14)

set(GCC_FLAGS "-std=c++14 -O2 -lOpenCL")
set(OpenCL_LIBRARIES "-lOpenCL")

# This should point to: "PATH-TO/level-zero/build/lib/libze_loader.so"
#set(ZE_LOADER $ENV{ZE_SHARED_LOADER})

add_definitions(${GCC_FLAGS})
include_directories(
        ./
        ${OPENCL_INCLUDE_DIRS}
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(main src/main.cpp src/instruction.cpp src/vm.cpp src/oclVM.cpp)
add_executable(gpuBenchmark src/gpuBenchmark.cpp src/instruction.cpp src/vm.cpp src/oclVM.cpp)
add_executable(testFPGA src/testFPGA.cpp src/instruction.cpp src/vm.cpp src/oclVM.cpp)

add_custom_target(build-time-make-directory ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory lib)

add_custom_command(
        TARGET main  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cl
        ${CMAKE_CURRENT_BINARY_DIR}/lib/)

target_link_libraries(main ${OpenCL_LIBRARIES})
target_link_libraries(gpuBenchmark ${OpenCL_LIBRARIES})
target_link_libraries(testFPGA ${OpenCL_LIBRARIES})