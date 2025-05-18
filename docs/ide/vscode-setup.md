# Optimized VSCode Setup for LLVM Development on macOS ARM64

This guide provides the optimal configuration for LLVM development in VSCode on macOS ARM64 platforms.

---

## Summary

### 1. Updated compile_commands.json

This file tells language servers exactly how your code is compiled, including all compiler flags and include paths. I've updated it to match your exact build command with all necessary LLVM flags. This is critical for proper code navigation and IntelliSense.

### 2. Enhanced VSCode Configuration Files

I've created or updated several key configuration files:

- c_cpp_properties.json: Improved with proper LLVM macros and browse settings
- tasks.json: Added a build task that uses your optimal LLVM compilation command
- launch.json: Added debugging configuration for your project

### 3. Comprehensive Documentation
I've created a detailed guide at docs/ide/vscode-setup.md that explains:

- How these configuration files work together
- Technical details about symbol resolution specific to ARM64
- Advanced performance optimizations
- Recommended extensions and configurations

### 4. Technical Benefits
This setup provides several technical advantages for LLVM development:

- Accurate Symbol Resolution: The compiler flags in compile_commands.json ensure IntelliSense correctly understands LLVM's complex C++ code
- Proper ABI Configuration: Ensures code navigation works with ARM64-specific symbol mangling
- Integrated Build System: The tasks.json file lets you build with Cmd+Shift+B using the exact same command that works at the terminal
- Streamlined Debugging: The launch configuration handles debugging with LLDB, making it easy to step through your code

To take full advantage of this setup, I recommend installing the clangd extension, which works better with complex C++ template code in LLVM than the standard C/C++ extension.

---

## Core Configuration Files

### 1. compile_commands.json

This database file tells language servers exactly how your files are compiled, including all compiler flags, include paths, and library dependencies.

```json
[
  {
    "directory": "/Users/jerry/eva-llvm",
    "command": "clang++ -std=c++17 -I/Users/jerry/toolchains/llvm-main/include -fno-exceptions -fno-rtti -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS main.cpp -o eva-llvm -L/Users/jerry/toolchains/llvm-main/lib -lLLVMCore -lLLVMSupport -lLLVMDemangle -lLLVMBinaryFormat -lLLVMRemarks -lLLVMBitstreamReader -lLLVMTargetParser -Wl,-undefined,dynamic_lookup",
    "file": "main.cpp"
  }
]
```

**How it works:**
- Language servers (clangd, ccls) read this file to understand build configuration
- VSCode uses this to provide accurate IntelliSense, code navigation, and diagnostics
- It ensures the editor knows about all include paths, compiler flags, and preprocessor definitions

**Generate from CMake:**
```bash
# In your build directory
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
# Copy to project root if using clangd
cp compile_commands.json ..
```

**Generate from direct compilation:**
```bash
# Using compiledb tool
pip install compiledb
compiledb -n make
```

### 2. c_cpp_properties.json

VSCode-specific configuration file located in `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "Mac",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/src/**",
                "~/toolchains/llvm-main/include"
            ],
            "defines": [
                "__STDC_CONSTANT_MACROS",
                "__STDC_FORMAT_MACROS",
                "__STDC_LIMIT_MACROS"
            ],
            "macFrameworkPath": [
                "/Applications/Xcode-beta.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks"
            ],
            "compilerPath": "/usr/bin/clang++",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "macos-clang-arm64",
            "compileCommands": "${workspaceFolder}/compile_commands.json",
            "browse": {
                "path": [
                    "${workspaceFolder}",
                    "~/toolchains/llvm-main/include"
                ],
                "limitSymbolsToIncludedHeaders": true,
                "databaseFilename": "${workspaceFolder}/.vscode/browse.vc.db"
            }
        }
    ],
    "version": 4
}
```

## Essential VSCode Extensions

1. **clangd** (preferred) or **C/C++** extension
   - clangd provides more responsive code intelligence for large codebases like LLVM
   - Handles complex template code better than the standard C/C++ extension

2. **CMake Tools**
   - Integrates CMake build system with VSCode
   - Simplifies build configuration and target selection

3. **CodeLLDB**
   - Provides debugging support for LLVM applications

4. **Clang-Format**
   - Automatic code formatting according to LLVM style guide

## Language Server Protocol (LSP) Setup

### clangd Configuration (Recommended)

Create a `.clangd` file in your project root:

```yaml
CompileFlags:
  CompilationDatabase: .
  Add: 
    - -fno-exceptions
    - -fno-rtti
    - -std=c++17
    - --cuda-gpu-arch=sm_75
    - -D__STDC_CONSTANT_MACROS
    - -D__STDC_FORMAT_MACROS
    - -D__STDC_LIMIT_MACROS

Diagnostics:
  UnusedIncludes: Strict
  ClangTidy:
    Add: 
      - llvm-*
      - clang-analyzer-*
    Remove: 
      - clang-analyzer-cplusplus.NewDeleteLeaks

InlayHints:
  Enabled: true
  ParameterNames: true
  DeducedTypes: true

Hover:
  ShowAKA: true

Index:
  Background: Build
```

### Tasks Configuration

Create a `.vscode/tasks.json` file to automate LLVM builds:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build LLVM Project",
            "type": "shell",
            "command": "clang++ -o eva-llvm main.cpp `llvm-config --cxxflags --ldflags --libs core support` -Wl,-rpath,`llvm-config --libdir` -Wl,-undefined,dynamic_lookup",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always",
                "panel": "new"
            },
            "problemMatcher": "$gcc"
        }
    ]
}
```

### Launch Configuration for Debugging

Create a `.vscode/launch.json` file for debugging:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug LLVM Project",
            "type": "lldb",
            "request": "launch",
            "program": "${workspaceFolder}/eva-llvm",
            "args": [],
            "cwd": "${workspaceFolder}",
            "preLaunchTask": "Build LLVM Project"
        }
    ]
}
```

## Advanced Performance Optimizations

### Symbol Indexing Configuration

For large LLVM-based projects, optimize indexing with:

```json
// settings.json
{
    "clangd.arguments": [
        "--background-index",
        "--clang-tidy",
        "--header-insertion=iwyu",
        "--completion-style=detailed",
        "--pch-storage=memory"
    ],
    "editor.semanticHighlighting.enabled": true
}
```

### Memory Management

When working with large LLVM projects:

```json
// settings.json
{
    "clangd.fallbackFlags": [
        "-xc++",
        "-std=c++17"
    ],
    "clangd.restartAfterCrash": true,
    "clangd.trace": "verbose",
    "clangd.onConfigChanged": "restart"
}
```

## Technical Implementation Details

### How compile_commands.json Works with clangd

1. **Accurate Symbol Resolution**:
   - clangd parses compile_commands.json to understand exact build configuration
   - Matches preprocessor state between editor and compiler
   - Ensures correct macro expansion and conditional compilation

2. **Include Path Resolution**:
   - Uses exact include search paths from actual compilation
   - Properly handles system vs. project includes
   - Maintains correct include order for header resolution

3. **Diagnostics Integration**:
   - Provides compiler warnings/errors directly in editor
   - Accurate error messages reflecting actual build configuration
   - Live diagnostics during editing without needing to compile

4. **Symbol Database Generation**:
   - Builds global symbol index based on compilation database
   - Enables "Go to Definition" across entire codebase
   - Powers accurate code completion for LLVM APIs

### ARM64 macOS Specifics

1. **Path Normalization**:
   - Handles `~` expansion in include paths
   - Manages architecture-specific include paths

2. **ABI Awareness**:
   - Ensures IntelliSense matches ARM64 ABI expectations
   - Proper handling of ARM NEON intrinsics and alignment requirements

3. **System SDK Integration**:
   - Correctly finds macOS SDK headers
   - Supports Xcode command-line tools integration
