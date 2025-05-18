#include "./src/EvaLLVM.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
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
    return 0;
}