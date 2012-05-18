/**

	File:		NUnaryOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NUnaryOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NUnaryOperator.h"

AsmBlock* NUnaryOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// get type
	IType* rhsType = this->rhs.getExpressionType(context);	
	
	// Move the value into A
	*block <<   *rhs;
	delete rhs;

	// Now do the appropriate operation.
	AsmBlock* compiledOp;
	switch (this->op)
	{
		case ADD:
			/* TODO integer promotion */
			compiledOp = rhsType->plus('A');
			break;

			/* unary negative:  "A = -B" */
		case SUBTRACT:
			// A = 0 - A
			compiledOp = rhsType->minus('A');
			break;

			/* unary bitwise negate:  "A = ~B" */
		case BITWISE_NEGATE:
			compiledOp = rhsType->bnot('A');
			break;
			/* boolean negate: A = !B  */
		case NEGATE:
			compiledOp = rhsType->lnot('A');
			break;

		default:
			throw new CompilerException(this->line, this->file, "Unknown unary operations requested.");
	}
	*block << *compiledOp;

	return block;
}

AsmBlock* NUnaryOperator::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an unary operator.");
}

IType* NUnaryOperator::getExpressionType(AsmGenerator& context)
{
	// A unary operator has the type of it's expression.
	return this->rhs.getExpressionType(context);
}
