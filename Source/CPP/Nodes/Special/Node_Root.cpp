// Copyright Chris Sixsmith 2020.

#include "Node_Root.h"

shared_ptr<Node> Node_Root::Clone() const
{
	return make_shared<Node_Root>(*this);
}

string Node_Root::GetDisplayName() const
{
	return "Root";
}

VarType Node_Root::GetReturnType() const
{
	return VarType::Invalid;
}

DECLARE_NODE(Node_Root);
