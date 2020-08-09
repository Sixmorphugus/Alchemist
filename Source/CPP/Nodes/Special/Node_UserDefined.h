// Copyright Chris Sixsmith 2020.

#pragma once

#include "Libs.h"
#include "Nodes/Nodes.h"

/**
 * Root nodes define a novel pattern that the function can accept.
 * They have a set of arguments that are expected, including:
 * - "Return" expression.
 * - Guard expression (expects a bool)
 * - Pattern for each function argument including variables to match the input to if any.
 */
class Node_UserDefined : public Node
{
public:
	Node_UserDefined(shared_ptr<Function> FuncIn);

	// Node interface.
	virtual string GetDisplayName() const override;
	virtual string GetCategory() const override { return "Your Program"; }
	virtual shared_ptr<Node> Clone() const override;
	virtual void Draw(const Alchemist* Instance, const Point& Position, bool IsPreview = false) const override;
	virtual bool Emit(string& Output, vector<CompilationProblem>& Problems) override;
	// End of Node interface.

protected:
	// Node interface.
	virtual void OnModuleChanged() override; // need to respond to the function being deleted
	// End of Node interface.

	virtual void SetupArgs();

private:
	weak_ptr<Function> Func;
};
