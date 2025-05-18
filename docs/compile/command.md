# LLVM Compilation on macOS ARM64: Technical Reference

## Minimal Working Command

```bash
clang++ -o eva-llvm main.cpp `llvm-config --cxxflags --ldflags --libs core` -Wl,-undefined,dynamic_lookup
```

## Core Components Analysis

### 1. Compiler Invocation
- `clang++`: C++ compiler frontend for LLVM
- `-o eva-llvm`: Output binary name

### 2. LLVM Configuration
- `--cxxflags`: Compiler flags
  - `-std=c++17`: C++ standard required by LLVM
  - `-fno-exceptions`: Disables C++ exception handling (LLVM design choice)
  - `-fno-rtti`: Disables runtime type information (LLVM optimization)
  - `-D__STDC_*_MACROS`: Required preprocessor definitions

- `--ldflags`: Linker flags
  - `-L/path/to/llvm/lib`: Library search paths

- `--libs core`: Required LLVM libraries
  - LLVMCore: Core functionality including IR representation
  - Automatically pulls LLVMSupport and other dependencies

### 3. ARM64-Specific Linker Flag
- `-Wl,-undefined,dynamic_lookup`: Critical for ARM64 compatibility
  - `-Wl`: Pass to linker
  - `-undefined,dynamic_lookup`: Allow undefined symbols at link time

## Technical Explanation: ARM64 Symbol Resolution

### Symbol Resolution Process

1. **Compile-time (Static) Resolution**:
   - Normally, all symbols must be resolved during linking
   - Linker searches provided libraries for symbol definitions
   - Fails if any symbols remain undefined

2. **Runtime (Dynamic) Resolution**:
   - Some symbols can be deferred to runtime resolution
   - Dynamic linker (dyld) resolves symbols when program loads
   - Enables more flexible library loading and symbol binding

### Apple Silicon ARM64 Specifics

#### ABI Implementation Details
- ARM64 uses a modified Itanium C++ ABI with Apple extensions
- Symbol mangling differs from x86_64 implementations
- Two-level namespace binding is default (stricter than Linux)

#### LLVM Library Build Characteristics
1. **Static Libraries (.a files)**:
   - ARM64 LLVM static libs omit certain C++ runtime symbols
   - Assumption: Runtime library will provide these symbols
   - Examples: operator new/delete, std::terminate, RTTI functions

2. **Platform Library Integration**:
   - libc++ and system libraries provide C++ runtime on macOS
   - Dynamic lookup enables seamless binding to these libraries

#### Technical Root Causes
1. **Library Design Decision**:
   - LLVM static libraries built without full C++ runtime inclusion
   - Reduces library size and avoids symbol duplication
   - Assumes platform-specific runtime resolution

2. **Apple Silicon Architecture Shift**:
   - M1/M2/M3 chips use ARM64 architecture
   - C++ ABI specific to Apple's ARM implementation
   - Stricter link-time checking than x86_64

## Alternative Compilation Methods

### CMake Approach (Recommended for Complex Projects)
```cmake
# CMakeLists.txt excerpt
cmake_minimum_required(VERSION 3.13)
project(EvaLLVM)

# LLVM package location
set(LLVM_DIR "~/toolchains/llvm-main/lib/cmake/llvm")

# Find and configure LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Set C++ standard required by LLVM
set(CMAKE_CXX_STANDARD 17)

# Create executable
add_executable(eva-llvm main.cpp)

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs support core)
target_link_libraries(eva-llvm ${llvm_libs})
```

### Extended Command Line (Fully Specified)
```bash
clang++ -o eva-llvm main.cpp -I/path/to/llvm/include -L/path/to/llvm/lib \
  -lLLVMCore -lLLVMSupport -lLLVMDemangle -lLLVMBinaryFormat \
  -lLLVMRemarks -lLLVMBitstreamReader -lLLVMTargetParser \
  -Wl,-rpath,/path/to/llvm/lib -Wl,-undefined,dynamic_lookup \
  -std=c++17 -fno-exceptions -fno-rtti
```

## Addressing Common Warning

```
ld: warning: reexported library with install name '@rpath/libunwind.1.dylib' found at 
'/path/to/libunwind.1.0.dylib' couldn't be matched with any parent library and will be linked directly
```

This warning occurs because:
1. libunwind.dylib is designed to be reexported by another library
2. Direct linking bypasses expected parent library
3. Functionally benign - linking works correctly despite the warning

## Cross-Platform Comparison

### Linux vs macOS ARM64

|                     | Linux                           | macOS ARM64                       |
|---------------------|----------------------------------|----------------------------------|
| Symbol Resolution   | More lenient                    | Stricter two-level namespace      |
| Library Structure   | Includes more runtime symbols   | Assumes system-provided runtime   |
| Default Linking     | Often accepts undefined symbols | Requires all symbols be resolved  |
| Symbol Visibility   | Simple namespace                | Two-level namespace with version  |
| C++ Runtime        | Often statically linked         | Dynamically linked by default     |

## Rebuild Options for LLVM 

To avoid dynamic lookup requirements, rebuild LLVM with:

```bash
cmake -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON [other options] /path/to/llvm/source
```

This creates a single shared library containing all components, avoiding static library limitations at the cost of larger binary size and longer build times.```

2. LLVM Configuration (llvm-config)

```bash
`llvm-config --cxxflags --ldflags --libs core`
```

3. Dynamic Symbol Resolution

```bash
-Wl,-undefined,dynamic_lookup
```

## About the Warning

The warning about libunwind is benign and can be safely ignored:

```bash
ld: warning: reexported library with install name '@rpath/libunwind.1.dylib' found at '/Users/jerry/toolchains/llvm-main/lib/libunwind.1.0.dylib' couldn't be matched with any parent library and will be linked directly
```

which can be solved with

```bash
-L ~/toolchains/llvm-main/lib -lunwind
```

Explicit libunwind linking: By explicitly including -L ~/toolchains/llvm-main/lib -lunwind, we correctly handle the libunwind library that was causing the warning


## Note

- Proper rpath setting: The -Wl,-rpath flag ensures your executable can find all necessary runtime libraries

- ARM64 compatibility: -Wl,-undefined,dynamic_lookup handles the ARM64-specific linking challenges