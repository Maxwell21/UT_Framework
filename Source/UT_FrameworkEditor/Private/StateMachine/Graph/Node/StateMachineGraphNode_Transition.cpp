/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphNode_Transition.h"
#include "StateMachineGraphNode.h"
#include "StateMachineTransition_Default.h"
#include "StateMachineState_Default.h"
#include "StateMachineGraph.h"
#include "StateMachineUtilities.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "FrameworkGenerics.h"
#include "BlueprintEditorUtils.h"
#include "KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "StateMachineGraphSchema"

UStateMachineGraphNode_Transition::UStateMachineGraphNode_Transition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->bCanRenameNode = false;
}

void UStateMachineGraphNode_Transition::ReconstructNode()
{
	this->InputPin = this->Pins[0];
	this->OutputPin = this->Pins[1];

	FGenericNodeData NodeData = this->Transition->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

FText UStateMachineGraphNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	UStateMachineGraphNode* PrevState = GetPreviousState();
	UStateMachineGraphNode* NextState = GetNextState();

	if ((PrevState != nullptr) && (NextState != nullptr))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("PrevState"), FText::FromString(PrevState->GetStateName()));
		Args.Add(TEXT("NextState"), FText::FromString(NextState->GetStateName()));

		return FText::Format(LOCTEXT("StateMachinePrevStateToNewState", "{PrevState} to {NextState}"), Args);
	}

	return FText();
}

void UStateMachineGraphNode_Transition::AllocateDefaultPins()
{
	InputPin = CreatePin(EGPD_Input, TEXT("Transition"), FString(), nullptr, TEXT(""));
	InputPin->bHidden = true;
	OutputPin = CreatePin(EGPD_Output, TEXT("Transition"), FString(), nullptr, TEXT(""));
	OutputPin->bHidden = true;
}

UObject* UStateMachineGraphNode_Transition::GetJumpTargetForDoubleClick() const
{
	return this->Transition->TransitionGraph;
}

bool UStateMachineGraphNode_Transition::CanJumpToDefinition() const
{
	return this->Transition->TransitionGraph != nullptr;
}

void UStateMachineGraphNode_Transition::JumpToDefinition() const
{
	if (UObject* HyperlinkTarget = GetJumpTargetForDoubleClick())
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
}

void UStateMachineGraphNode_Transition::DestroyNode()
{
	UEdGraph* GraphToRemove = this->Transition->TransitionGraph;
	UStateMachineGraph* StateMachineGraph = Cast<UStateMachineGraph>(GetGraph());
	
	this->Transition->TransitionGraph = nullptr;

	// Remove graph related to this transition
	if (GraphToRemove && StateMachineGraph)
		FBlueprintEditorUtils::RemoveGraph(StateMachineGraph->StateMachineBlueprint, GraphToRemove, EGraphRemoveFlags::Recompile);

	this->Transition->MarkPendingKill();

	UEdGraphNode::DestroyNode();
}

void UStateMachineGraphNode_Transition::CreateBoundGraph()
{
	if (UStateMachineGraph* Graph = Cast<UStateMachineGraph>(GetGraph()))
		FStateMachineUtilities::AddNewBoundedGraphToTransition(Graph, this);
}

UStateMachineGraphNode* UStateMachineGraphNode_Transition::GetPreviousState() const
{
	return InputPin->LinkedTo.Num() > 0 ? Cast<UStateMachineGraphNode>(InputPin->LinkedTo[0]->GetOwningNode()) : nullptr;
}

UStateMachineGraphNode* UStateMachineGraphNode_Transition::GetNextState() const
{
	return OutputPin->LinkedTo.Num() > 0 ? Cast<UStateMachineGraphNode>(OutputPin->LinkedTo[0]->GetOwningNode()) : nullptr;
}

void UStateMachineGraphNode_Transition::CreateConnections(UStateMachineGraphNode* NodeA, UStateMachineGraphNode* NodeB)
{
	this->LinkNode(NodeA, NodeB);

	// Add transition to the from state
	NodeA->State->Transitions.Add(this->Transition);

	// Create transition graph
	this->CreateBoundGraph();
}

void UStateMachineGraphNode_Transition::LinkNode(UStateMachineGraphNode* NodeA, UStateMachineGraphNode* NodeB)
{
	// Previous to this
	InputPin->Modify();
	InputPin->LinkedTo.Empty();

	NodeA->GetOutputPin()->Modify();
	InputPin->MakeLinkTo(NodeA->GetOutputPin());

	// This to next
	OutputPin->Modify();
	OutputPin->LinkedTo.Empty();

	NodeB->GetInputPin()->Modify();
	OutputPin->MakeLinkTo(NodeB->GetInputPin());

	// Store state objects inside transition object
	this->Transition->FromState = NodeA->State;
	this->Transition->ToState = NodeB->State;
}

#undef LOCTEXT_NAMESPACE
