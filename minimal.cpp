#include <iostream>
#include <memory>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

int main() {
    // Simple LLVM program that just creates a module and prints it
    
    // Initialize LLVM components
    std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("MinimalExample", *context);
    std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);
    
    // Create a simple function declaration
    llvm::FunctionType *funcType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context),  // Return type
        {llvm::Type::getInt32Ty(*context), llvm::Type::getInt32Ty(*context)}, // Param types
        false); // Not vararg
        
    llvm::Function *func = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        "add", 
        *module);
    
    // Print the module IR
    module->print(llvm::outs(), nullptr);
    
    std::cout << "Successfully generated minimal LLVM IR" << std::endl;
    return 0;
}
