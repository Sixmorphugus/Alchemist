// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

/* Compile error info. */
struct CompilationProblem
{
	weak_ptr<Node> ProblemNode;
	string Problem;
};