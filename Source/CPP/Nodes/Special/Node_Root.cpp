// Copyright Chris Sixsmith 2020.

#include "Node_Root.h"

Node* Node_Root::Clone() const
{
	return new Node_Root(*this);
}

string Node_Root::GetDisplayName() const
{
	return "Root";
}

VarType Node_Root::GetReturnType() const
{
	return VarType::Invalid;
}

DECLARE_NODE(Node_Root, Node_Root());