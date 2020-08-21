// Copyright Chris Sixsmith 2020.

#pragma once

#include "Nodes/Nodes.h"

/**
 * Root nodes define a novel pattern that the function can accept.
 * They have a set of arguments that are expected, including:
 * - "Return" expression.
 * - Guard expression (expects a bool)
 * - Pattern for each function argument including variables to match the input to if any.
 */
class Node_Root : public Node
{
public:
	Node_Root();

	// Node interface.
	virtual string GetDisplayName() const override { return "Root"; }
	virtual string GetCategory() const override { return "Basic"; }
	virtual shared_ptr<Node> Clone() const override;
	virtual void Draw(const Alchemist* Instance, const Point& Position, bool IsPreview = false) const override;
	virtual bool EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path) override;
	// End of Node interface.

protected:
	// Node interface.
	virtual void OnFunctionChanged() override;
	// End of Node interface.
};
