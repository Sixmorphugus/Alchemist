// Copyright Chris Sixsmith 2020.

#pragma once

#include "Nodes/Nodes.h"

/**
 * Term nodes are a constant value.
 * There are different kinds for different types.
 */
class Node_Variable : public Node
{
public:
	// Node interface.
	virtual shared_ptr<Node> Clone() const override;
	virtual string GetDisplayName() const override { return "Variable (" + Name + ")"; }
	virtual string GetCategory() const override { return "Basic"; }
	virtual void Draw(const Alchemist* Instance, const Point& Position, bool IsPreview = false) const override;
	//virtual void Load(const ifstream& FileStream) override;
	//virtual void Save(const ofstream& FileStream) const override;
	//virtual size_t GetDataSize() const override;
	virtual bool EmitInternal(string& Output, vector<CompilationProblem>& Problems, vector<shared_ptr<Node>> Path) override;
	virtual void HandleTextInput(const SDL_Event& Event) override;
	virtual void HandleKeyPress(const SDL_Event& Event) override;
	// End of Node interface.

	void SetName(string NewValue) { Name = NewValue; }
	string GetName() { return Name; }

protected:
	string Name = "I";
};
