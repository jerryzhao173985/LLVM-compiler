#ifndef EVA_LLVM_H
#define EVA_LLVM_H

#include <iostream>
#include <memory>
#include <string>

// Include LLVM headers using quotes as in the minimal example
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

class EvaLLVM {
public:
    /**
     * @brief Default constructor for EvaLLVM
     */
    EvaLLVM() {
        // Create LLVM context, module and builder (just like in minimal.cpp)
        context = std::make_unique<llvm::LLVMContext>();
        module = std::make_unique<llvm::Module>("EvaLLVM", *context);
        builder = std::make_unique<llvm::IRBuilder<>>(*context);
        
        std::cout << "EvaLLVM initialized successfully" << std::endl;
    }
    
    /**
     * @brief Destructor to clean up resources
     */
    ~EvaLLVM() = default;

    /**
     * @brief Process input source code
     * @param input Source code or filename
     * @param isFilename Whether the input is a filename
     */
    void exec(const std::string &input, bool isFilename = false) {
        // Log what we're doing
        if (isFilename) {
            std::cout << "Reading from file: " << input << std::endl;
        } else {
            std::cout << "Processing inline code" << std::endl;
        }

        // Create a simple function to demonstrate it works
        createSampleFunction();

        // Print generated LLVM IR to console
        module->print(llvm::outs(), nullptr);

        // Save module IR to file
        saveModuleToFile("./out.ll");
    }

private:
    /**
     * @brief Create a simple add function
     */
    void createSampleFunction() {
        // Define function type: int add(int, int)
        llvm::FunctionType *funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(*context),  // Return type
            {llvm::Type::getInt32Ty(*context), llvm::Type::getInt32Ty(*context)}, // Param types
            false); // Not vararg
            
        // Create the function in the module
        llvm::Function *func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            "add", 
            *module);
            
        // Name the function arguments
        llvm::Argument *arg1 = func->getArg(0);
        llvm::Argument *arg2 = func->getArg(1);
        arg1->setName("a");
        arg2->setName("b");
        
        // Create a basic block
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(entry);
        
        // Add the two arguments
        llvm::Value *sum = builder->CreateAdd(arg1, arg2, "sum");
        
        // Return the result
        builder->CreateRet(sum);
    }

    /**
     * @brief Save module IR to a file
     */
    void saveModuleToFile(const std::string &filename) {
        std::error_code errorCode;
        llvm::raw_fd_ostream outFile(filename, errorCode);
        if (errorCode) {
            std::cerr << "Could not open file: " << errorCode.message() << std::endl;
            return;
        }
        module->print(outFile, nullptr);
        std::cout << "LLVM IR saved to " << filename << std::endl;
    }

    // Core LLVM components
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
};

#endif // EVA_LLVM_H