// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/node-properties.h"

#include "src/compiler/common-operator.h"
#include "src/compiler/operator-properties.h"

namespace v8 {
namespace internal {
namespace compiler {

// static
int NodeProperties::PastValueIndex(Node* node) {
  return FirstValueIndex(node) + node->op()->ValueInputCount();
}


// static
int NodeProperties::PastContextIndex(Node* node) {
  return FirstContextIndex(node) +
         OperatorProperties::GetContextInputCount(node->op());
}


// static
int NodeProperties::PastFrameStateIndex(Node* node) {
  return FirstFrameStateIndex(node) +
         OperatorProperties::GetFrameStateInputCount(node->op());
}


// static
int NodeProperties::PastEffectIndex(Node* node) {
  return FirstEffectIndex(node) + node->op()->EffectInputCount();
}


// static
int NodeProperties::PastControlIndex(Node* node) {
  return FirstControlIndex(node) + node->op()->ControlInputCount();
}


// static
Node* NodeProperties::GetValueInput(Node* node, int index) {
  DCHECK(0 <= index && index < node->op()->ValueInputCount());
  return node->InputAt(FirstValueIndex(node) + index);
}


// static
Node* NodeProperties::GetContextInput(Node* node) {
  DCHECK(OperatorProperties::HasContextInput(node->op()));
  return node->InputAt(FirstContextIndex(node));
}


// static
Node* NodeProperties::GetFrameStateInput(Node* node) {
  DCHECK(OperatorProperties::HasFrameStateInput(node->op()));
  return node->InputAt(FirstFrameStateIndex(node));
}


// static
Node* NodeProperties::GetEffectInput(Node* node, int index) {
  DCHECK(0 <= index && index < node->op()->EffectInputCount());
  return node->InputAt(FirstEffectIndex(node) + index);
}


// static
Node* NodeProperties::GetControlInput(Node* node, int index) {
  DCHECK(0 <= index && index < node->op()->ControlInputCount());
  return node->InputAt(FirstControlIndex(node) + index);
}


// static
bool NodeProperties::IsValueEdge(Edge edge) {
  Node* const node = edge.from();
  return IsInputRange(edge, FirstValueIndex(node),
                      node->op()->ValueInputCount());
}


// static
bool NodeProperties::IsContextEdge(Edge edge) {
  Node* const node = edge.from();
  return IsInputRange(edge, FirstContextIndex(node),
                      OperatorProperties::GetContextInputCount(node->op()));
}


// static
bool NodeProperties::IsFrameStateEdge(Edge edge) {
  Node* const node = edge.from();
  return IsInputRange(edge, FirstFrameStateIndex(node),
                      OperatorProperties::GetFrameStateInputCount(node->op()));
}


// static
bool NodeProperties::IsEffectEdge(Edge edge) {
  Node* const node = edge.from();
  return IsInputRange(edge, FirstEffectIndex(node),
                      node->op()->EffectInputCount());
}


// static
bool NodeProperties::IsControlEdge(Edge edge) {
  Node* const node = edge.from();
  return IsInputRange(edge, FirstControlIndex(node),
                      node->op()->ControlInputCount());
}


// static
void NodeProperties::ReplaceContextInput(Node* node, Node* context) {
  node->ReplaceInput(FirstContextIndex(node), context);
}


// static
void NodeProperties::ReplaceControlInput(Node* node, Node* control) {
  node->ReplaceInput(FirstControlIndex(node), control);
}


// static
void NodeProperties::ReplaceEffectInput(Node* node, Node* effect, int index) {
  DCHECK(index < node->op()->EffectInputCount());
  return node->ReplaceInput(FirstEffectIndex(node) + index, effect);
}


// static
void NodeProperties::ReplaceFrameStateInput(Node* node, Node* frame_state) {
  DCHECK(OperatorProperties::HasFrameStateInput(node->op()));
  node->ReplaceInput(FirstFrameStateIndex(node), frame_state);
}


// static
void NodeProperties::RemoveNonValueInputs(Node* node) {
  node->TrimInputCount(node->op()->ValueInputCount());
}


// static
void NodeProperties::ReplaceWithValue(Node* node, Node* value, Node* effect) {
  DCHECK(node->op()->ControlOutputCount() == 0);
  if (!effect && node->op()->EffectInputCount() > 0) {
    effect = NodeProperties::GetEffectInput(node);
  }

  // Requires distinguishing between value and effect edges.
  for (Edge edge : node->use_edges()) {
    if (IsEffectEdge(edge)) {
      DCHECK_NOT_NULL(effect);
      edge.UpdateTo(effect);
    } else {
      edge.UpdateTo(value);
    }
  }
}


// static
Node* NodeProperties::FindProjection(Node* node, size_t projection_index) {
  for (auto use : node->uses()) {
    if (use->opcode() == IrOpcode::kProjection &&
        ProjectionIndexOf(use->op()) == projection_index) {
      return use;
    }
  }
  return nullptr;
}


// static
bool NodeProperties::AllValueInputsAreTyped(Node* node) {
  int input_count = node->op()->ValueInputCount();
  for (int index = 0; index < input_count; ++index) {
    if (!IsTyped(GetValueInput(node, index))) return false;
  }
  return true;
}


// static
bool NodeProperties::IsInputRange(Edge edge, int first, int num) {
  if (num == 0) return false;
  int const index = edge.index();
  return first <= index && index < first + num;
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
