#include <llvm-c/Core.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdbool.h>
#include "defines.h"

static bool inittedLlvm = false;

static void initLlvm()
{
    if(inittedLlvm) return;
    else inittedLlvm = true;

    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();
}

bool toObjectFile(LLVMModuleRef module, const char *triple, LLVMTargetMachineRef target, const char *objFName)
{
    initLlvm();

    LLVMSetModuleDataLayout(module, LLVMCreateTargetDataLayout(target));
    LLVMSetTarget(module, triple);

    char *errStr;
    bool success = LLVMTargetMachineEmitToFile(target, module, objFName, LLVMObjectFile, &errStr);
    if(success) return true;
    else
    {
        ERR("Failed to emit object code: %s\n", errStr);
        LLVMDisposeMessage(errStr);
        return false;
    }
}

bool getTargetMachine(const char *triple, LLVMTargetMachineRef *machine)
{
    initLlvm();

    char *errStr;
    LLVMTargetRef target;
    bool foundTarget = LLVMGetTargetFromTriple(triple, &target, &errStr);
    if(!foundTarget)
    {
        ERR("Failed to find target for triple: %s\n", errStr);
        LLVMDisposeMessage(errStr);
        return false;
    }

    *machine = LLVMCreateTargetMachine(
        target,
        triple,
        "generic",
        "",
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault,
        LLVMCodeModelDefault
    );
    
    return true;
}