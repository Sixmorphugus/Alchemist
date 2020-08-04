// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"

/* Compile error info. */
struct CompilationProblem
{
	shared_ptr<Node> ProblemNode;
	string Problem;
};