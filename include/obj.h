#pragma once

#include <stdbool.h>
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

bool toObjectFile(LLVMModuleRef module, const char *triple, LLVMTargetMachineRef target, const char *objFName);

bool getTargetMachine(const char *triple, LLVMTargetMachineRef *machine);