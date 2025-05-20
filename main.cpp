#include "./src/EvaLLVM.h"
#include <iostream>
#include <string>
#include "doctest.h"

TEST_CASE("Test EvaLLVM") {
    // Sample program source code
    std::string program = R"(
        42
    )";

    // Initialize the EvaLLVM compiler
    EvaLLVM eva;

    // Process the program source (not a filename)
    // Second parameter is false to indicate this is source code, not a filename
    eva.exec(program, false);  

    std::cout << "LLVM IR has been generated and saved to out.ll" << std::endl;

    // Check if the output file exists
    std::ifstream infile("out.ll");
    CHECK(infile.good());
}

int main(int argc, char *argv[]) {
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();
    if (context.shouldExit()) {
        return res;
    }
    return res;
}
