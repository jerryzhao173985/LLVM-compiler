# LLVM Compiler Project for macOS ARM64

A lightweight LLVM-based compiler implementation specifically optimized for macOS ARM64 (Apple Silicon) architecture. This project demonstrates how to properly integrate LLVM libraries on Apple Silicon and handle the unique linking and symbol resolution challenges specific to ARM64.

## Technical Overview

This project implements a simple LLVM-based compiler that:

1. Creates and manipulates LLVM IR programmatically
2. Demonstrates proper linking techniques for macOS ARM64
3. Provides optimized VSCode configuration for LLVM development
4. Includes documentation of ARM64-specific compilation challenges

## Repository Structure

```
.
├── Makefile                # Optimized for macOS ARM64 LLVM compilation
├── docs/
│   ├── compile/
│   │   └── command.md      # Technical reference on LLVM compilation
│   └── ide/
│       └── vscode-setup.md # VSCode configuration guide
├── src/
│   └── EvaLLVM.h           # Main compiler class definition
├── main.cpp                # Entry point and demo application
├── minimal.cpp             # Minimal LLVM IR generation example
├── .clangd                 # clangd language server configuration
├── .clang-format           # LLVM coding style formatter
└── .vscode/                # VSCode configuration files
    ├── c_cpp_properties.json
    ├── tasks.json          # Build tasks
    └── launch.json         # Debugging configuration
```

## Prerequisites

- macOS running on Apple Silicon (M1/M2/M3)
- LLVM installed via Homebrew or built from source
- Clang C++ compiler
- VSCode with C++ and clangd extensions (recommended)

## Installation and Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/jerryzhao173985/LLVM-compiler.git
   cd LLVM-compiler
   ```

2. Ensure LLVM is properly installed and available in your path:
   ```bash
   # Check LLVM installation
   llvm-config --version
   ```

3. Verify `llvm-config` is properly configured to point to your toolchain:
   ```bash
   # Update Makefile if necessary with your LLVM path
   LLVM_CONFIG = ~/toolchains/llvm-main/bin/llvm-config
   ```

## Building the Project

```bash
# Build using optimized makefile
make clean && make

# Run the compiled executable
./eva-llvm
```

This will:
1. Compile the project with proper ARM64 flags
2. Set the runtime path for dynamic library loading
3. Generate a sample LLVM IR module
4. Save the IR to `out.ll`

## Key Technical Features

### ARM64-Specific Linking

The project demonstrates the correct way to link LLVM libraries on ARM64:

```bash
-Wl,-undefined,dynamic_lookup -Wl,-rpath,$(LLVM_LIBDIR)
```

- `-Wl,-undefined,dynamic_lookup`: Enables runtime symbol resolution for ARM64
- `-Wl,-rpath,$(LLVM_LIBDIR)`: Sets the runtime path for shared libraries

### VSCode Integration

Complete VSCode configuration for efficient LLVM development:

1. **Language Server**: Optimized `clangd` configuration in `.clangd`
2. **IntelliSense**: Proper include paths and macros in `c_cpp_properties.json`
3. **Build Tasks**: LLVM compilation commands in `tasks.json`
4. **Debugging**: LLDB integration in `launch.json`

### LLVM IR Generation

The `EvaLLVM` class demonstrates:

1. Proper LLVM context, module, and IRBuilder initialization
2. Function and basic block creation
3. IR instruction insertion
4. Module serialization

## Documentation

Comprehensive documentation is available in the `docs/` directory:

- [LLVM Compilation Reference](docs/compile/command.md): Detailed explanation of LLVM compilation on macOS ARM64
- [VSCode Setup Guide](docs/ide/vscode-setup.md): Complete configuration guide for VSCode with LLVM

## Development Workflow

1. Write your code in the `src/` directory
2. Build using `make`
3. Run tests with `./eva-llvm`
4. Analyze generated LLVM IR in `out.ll`

## Troubleshooting

### Linking Errors

If you encounter "missing symbol" errors, check that:
- You're using the `-Wl,-undefined,dynamic_lookup` flag
- The RPATH is correctly set to your LLVM library directory
- All required LLVM libraries are linked

### VSCode Issues

For IntelliSense problems:
- Ensure the `.clangd` configuration is in your project root
- Check that `compile_commands.json` has the correct compiler flags
- Restart the clangd server: Cmd+Shift+P → "clangd: Restart Language Server"

## License

MIT

## Acknowledgments

This project builds upon the LLVM compiler infrastructure project.
