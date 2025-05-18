CXX = clang++
LLVM_CONFIG = ~/toolchains/llvm-main/bin/llvm-config

# ARM64-specific flags for macOS
CXXFLAGS = -std=c++17 \
	-I$(shell $(LLVM_CONFIG) --includedir) \
	-fno-exceptions \
	-fno-rtti \
	-D__STDC_CONSTANT_MACROS \
	-D__STDC_FORMAT_MACROS \
	-D__STDC_LIMIT_MACROS

# Get LLVM library directory for RPATH setting
LLVM_LIBDIR = $(shell $(LLVM_CONFIG) --libdir)

# Use the dynamic lookup flag for ARM64 macOS linking
# Add RPATH to find runtime libraries
LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags) \
	$(shell $(LLVM_CONFIG) --libs core support) \
	-Wl,-undefined,dynamic_lookup \
	-Wl,-rpath,$(LLVM_LIBDIR)

SOURCES = $(wildcard src/*.cpp) main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = eva-llvm

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
