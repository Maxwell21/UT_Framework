/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphFactory.h"
#include "StateMachineGraphSchema.h"
#include "Policy/StateMachineConnectionDrawingPolicy.h"

#include "Node/StateMachineGraphNode.h"
#include "Node/StateMachineGraphNode_Entry.h"
#include "Node/StateMachineGraphNode_Transition.h"

#include "Widget/SStateMachineGraphNode.h"
#include "Widget/SStateMachineGraphNode_Entry.h"
#include "Widget/SStateMachineGraphNode_Transition.h"

TSharedPtr<class SGraphNode> FStateMachineGraphFactory::CreateNode(class UEdGraphNode* InNode) const
{
	if (UStateMachineGraphNode_Entry* EntryNode = Cast<UStateMachineGraphNode_Entry>(InNode))
		return SNew(SStateMachineGraphNode_Entry, EntryNode);
	else if (UStateMachineGraphNode_Transition* TransitionNode = Cast<UStateMachineGraphNode_Transition>(InNode))
		return SNew(SStateMachineGraphNode_Transition, TransitionNode);
	else if (UStateMachineGraphNode* Node = Cast<UStateMachineGraphNode>(InNode))
		return SNew(SStateMachineGraphNode, Node);

	return nullptr;
}

class FConnectionDrawingPolicy* FStateMachineGraphPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	if (Schema->IsA(UStateMachineGraphSchema::StaticClass()))
		return new FStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	
	return nullptr;
}
