// Copyright Chris Sixsmith 2020.

#pragma once

#include "Nodes/Nodes.h"

class Node_Root : public Node
{
	// Node interface.
	virtual Node* Clone() const override;
	virtual string GetDisplayName() const override;
	virtual VarType GetReturnType() const override;
	// End of Node interface.
};
