// Copyright Chris Sixsmith 2020.

#include "Node_Arithmetic.h"

DECLARE_NODE_CUSTOMNAME(Add, Node_BinaryOperator<BinaryOperatorTraits_Add>);
DECLARE_NODE_CUSTOMNAME(Sub, Node_BinaryOperator<BinaryOperatorTraits_Subtract>);
DECLARE_NODE_CUSTOMNAME(Multiply, Node_BinaryOperator<BinaryOperatorTraits_Multiply>);
DECLARE_NODE_CUSTOMNAME(Divide, Node_BinaryOperator<BinaryOperatorTraits_Divide>);

DECLARE_NODE_CUSTOMNAME(Equal, Node_BinaryOperator<BinaryOperatorTraits_Equal>);
DECLARE_NODE_CUSTOMNAME(NotEqual, Node_BinaryOperator<BinaryOperatorTraits_NotEqual>);
DECLARE_NODE_CUSTOMNAME(Greater, Node_BinaryOperator<BinaryOperatorTraits_Greater>);
DECLARE_NODE_CUSTOMNAME(GreaterEqual, Node_BinaryOperator<BinaryOperatorTraits_GreaterEqual>);
DECLARE_NODE_CUSTOMNAME(Less, Node_BinaryOperator<BinaryOperatorTraits_Less>);
DECLARE_NODE_CUSTOMNAME(LessEqual, Node_BinaryOperator<BinaryOperatorTraits_LessEqual>);

DECLARE_NODE_CUSTOMNAME(And, Node_BinaryOperator<BinaryOperatorTraits_And>);
DECLARE_NODE_CUSTOMNAME(Or, Node_BinaryOperator<BinaryOperatorTraits_Or>);
