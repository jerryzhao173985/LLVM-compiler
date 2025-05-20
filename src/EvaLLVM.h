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
#include "llvm/IR/Verifier.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeReader.h"

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

        // Create a more complex function with multiple basic blocks and control flow
        createComplexFunction();

        // Add global variables and manipulate them within functions
        addGlobalVariable();

        // Apply optimization passes
        applyOptimizationPasses();

        // Verify the correctness of the generated IR
        verifyModule();

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
     * @brief Create a more complex function with multiple basic blocks and control flow
     */
    void createComplexFunction() {
        // Define function type: int complexFunc(int, int)
        llvm::FunctionType *funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(*context),  // Return type
            {llvm::Type::getInt32Ty(*context), llvm::Type::getInt32Ty(*context)}, // Param types
            false); // Not vararg
            
        // Create the function in the module
        llvm::Function *func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            "complexFunc", 
            *module);
            
        // Name the function arguments
        llvm::Argument *arg1 = func->getArg(0);
        llvm::Argument *arg2 = func->getArg(1);
        arg1->setName("x");
        arg2->setName("y");
        
        // Create basic blocks
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context, "entry", func);
        llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*context, "then", func);
        llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*context, "else");
        llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*context, "ifcont");
        
        builder->SetInsertPoint(entry);
        
        // Create conditional branch
        llvm::Value *cond = builder->CreateICmpSGT(arg1, arg2, "cond");
        builder->CreateCondBr(cond, thenBB, elseBB);
        
        // Then block
        builder->SetInsertPoint(thenBB);
        llvm::Value *thenVal = builder->CreateAdd(arg1, arg2, "thenVal");
        builder->CreateBr(mergeBB);
        
        // Else block
        func->getBasicBlockList().push_back(elseBB);
        builder->SetInsertPoint(elseBB);
        llvm::Value *elseVal = builder->CreateSub(arg1, arg2, "elseVal");
        builder->CreateBr(mergeBB);
        
        // Merge block
        func->getBasicBlockList().push_back(mergeBB);
        builder->SetInsertPoint(mergeBB);
        llvm::PHINode *phi = builder->CreatePHI(llvm::Type::getInt32Ty(*context), 2, "iftmp");
        phi->addIncoming(thenVal, thenBB);
        phi->addIncoming(elseVal, elseBB);
        
        // Return the result
        builder->CreateRet(phi);
    }

    /**
     * @brief Add a global variable and manipulate it within functions
     */
    void addGlobalVariable() {
        // Create a global variable: int gVar = 42
        llvm::GlobalVariable *gVar = new llvm::GlobalVariable(
            *module,
            llvm::Type::getInt32Ty(*context),
            false,
            llvm::GlobalValue::ExternalLinkage,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 42),
            "gVar");
        
        // Create a function to manipulate the global variable
        llvm::FunctionType *funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(*context),  // Return type
            {}, // No parameters
            false); // Not vararg
            
        llvm::Function *func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            "manipulateGlobalVar", 
            *module);
            
        llvm::BasicBlock *entry = llvm::BasicBlock::Create(*context, "entry", func);
        builder->SetInsertPoint(entry);
        
        // Load the global variable
        llvm::Value *gVarVal = builder->CreateLoad(gVar->getValueType(), gVar, "gVarVal");
        
        // Increment the global variable
        llvm::Value *incVal = builder->CreateAdd(gVarVal, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 1), "incVal");
        
        // Store the incremented value back to the global variable
        builder->CreateStore(incVal, gVar);
        
        // Return void
        builder->CreateRetVoid();
    }

    /**
     * @brief Apply optimization passes to the module
     */
    void applyOptimizationPasses() {
        llvm::legacy::PassManager passManager;
        passManager.add(llvm::createConstantPropagationPass());
        passManager.add(llvm::createDeadCodeEliminationPass());
        passManager.add(llvm::createLoopUnrollPass());
        passManager.run(*module);
    }

    /**
     * @brief Verify the correctness of the generated IR
     */
    void verifyModule() {
        if (llvm::verifyModule(*module, &llvm::errs())) {
            std::cerr << "Error: module verification failed" << std::endl;
        } else {
            std::cout << "Module verified successfully" << std::endl;
        }
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
