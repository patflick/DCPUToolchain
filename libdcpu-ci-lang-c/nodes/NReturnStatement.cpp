/**

    File:           NReturnStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes
                    Patrick Flick

    Description:    Defines the NReturnStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NReturnStatement.h"

AsmBlock* NReturnStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Evaluate the expression (the expression will always put
    // it's output in register A).
    if (this->result != NULL)
    {
        AsmBlock* eval = this->result->compile(context);
        *block << *eval;
        delete eval;
    }

    // Free locals.
    *block <<  "    ADD SP, " << context.symbolTable->getCurrentScope().getLocalStackSize() << std::endl;

    // Return from this function.
    *block <<  "    SET X, " << context.symbolTable->getCurrentScope().getParameterStackSize() << std::endl;
    *block <<  "    SET PC, _stack_callee_return" << std::endl;

    return block;
}

AsmBlock* NReturnStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}
