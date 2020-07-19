// Copyright Chris Sixsmith 2020.

#include "Node_Term.h"

shared_ptr<Node> Node_Term_Int::Clone() const
{
	return make_shared<Node_Term_Int>(*this);
}

void Node_Term_Int::Draw(Alchemist* Instance, const Point& Position, bool IsPreview) const
{
	Node::Draw(Instance, Position, IsPreview);
}

void Node_Term_Int::Load(const ifstream& FileStream)
{
	// TODO
}

void Node_Term_Int::Save(const ofstream& FileStream) const
{
	// TODO
}

size_t Node_Term_Int::GetDataSize() const
{
	return sizeof(int);
}

DECLARE_NODE(Node_Term_Int);
