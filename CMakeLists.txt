cmake_minimum_required(VERSION 3.13)
project(EvaLLVM)

# Specify path to LLVM installation
set(LLVM_DIR "~/toolchains/llvm-main/lib/cmake/llvm")

# Find LLVM package
find_package(LLVM REQUIRED CONFIG)

# Print LLVM information
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM CMake configuration
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Set C++ standard required by LLVM
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include project directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
set(SOURCES
    main.cpp
)

# Create executable
add_executable(eva-llvm ${SOURCES})

# Get list of required LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core)

# Link with LLVM libraries
target_link_libraries(eva-llvm ${llvm_libs})

