// Copyright Chris Sixsmith 2020.

#pragma once

#include "Nodes/Nodes.h"

class Node_Root : public Node
{
public:
	// Node interface.
	virtual shared_ptr<Node> Clone() const override;
	virtual string GetDisplayName() const override;
	virtual VarType GetReturnType() const override;
	virtual void Draw(Alchemist* Instance, const Point& Position, bool IsPreview = false) const override;
	// End of Node interface.
};
