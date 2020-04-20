/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Transition.h"
#include "StateMachineGraph.h"
#include "StateMachineGraphSchema.h"
#include "StateMachineState_Default.h"
#include "StateMachineBlueprint.h"
#include "StateMachine.h"
#include "StateMachineUtilities.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"
#include "BlueprintEditorUtils.h"
#include "KismetEditorUtilities.h"

UStateMachineGraphNode::UStateMachineGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	IsRootNode(false),
	InputPin(nullptr),
	OutputPin(nullptr),
	State(nullptr)
{
	this->bCanRenameNode = true;
}

void UStateMachineGraphNode::SetIsRootNode(bool IsNewRootNode)
{
	this->IsRootNode = IsNewRootNode;
}

void UStateMachineGraphNode::ReconstructNode()
{
	this->InputPin = this->Pins[0];
	this->OutputPin = this->Pins[1];

	FGenericNodeData NodeData = this->State->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

void UStateMachineGraphNode::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin = CreatePin(EGPD_Input, TEXT("State"), FName(), nullptr, TEXT(""));
	OutputPin = CreatePin(EGPD_Output, TEXT("State"), FName(), nullptr, TEXT(""));
}

bool UStateMachineGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UStateMachineGraphSchema::StaticClass());
}

bool UStateMachineGraphNode::CanUserDeleteNode() const
{
	return !this->IsRootNode;
}

void UStateMachineGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	UStateMachineGraph* Graph = Cast<UStateMachineGraph>(GetGraph());
	if (Graph && FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
			FromPin->GetOwningNode()->NodeConnectionListChanged();
	}
}

void UStateMachineGraphNode::DestroyNode()
{
	UEdGraph* GraphToRemove = this->State->StateGraph;
	UStateMachineGraph* StateMachineGraph = Cast<UStateMachineGraph>(GetGraph());
	TArray<UStateMachineGraphNode_Transition*> Transitions;
	this->GetTransitionList(Transitions);

	this->State->StateGraph = nullptr;

	if (GraphToRemove && StateMachineGraph)
	{
		// Remove the graph related to this State
		FBlueprintEditorUtils::RemoveGraph(StateMachineGraph->StateMachineBlueprint, GraphToRemove, EGraphRemoveFlags::Recompile);

		// Remove all transition nodes related to this State
		for (auto TransNode : Transitions)
		{
			TransNode->DestroyNode();
		}
	}

	if (this->State)
	{
		// Destroy object and dereference it to the states array
		StateMachineGraph->StateMachineBlueprint->GetStateMachine()->GetGeneratedClass()->States.Remove(this->State);
		StateMachineGraph->StateMachineBlueprint->GetStateMachine()->RemoveStateByName(this->State->RuntimeData.Name);
		this->State->MarkPendingKill();
	}

	Super::DestroyNode();
}

void UStateMachineGraphNode::SaveNode()
{
	if (this->State)
	{
		FGenericNodeData NodeData;
		NodeData.NodePosX = this->NodePosX;
		NodeData.NodePosY = this->NodePosY;
		this->State->NodeData = NodeData;
	}
}

void UStateMachineGraphNode::OnNotifyGraphChanged()
{
	this->SaveNode();
}

void UStateMachineGraphNode::OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{

}

FLinearColor UStateMachineGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(0.609375f, 0.038051f, 0.044112f);
}

FText UStateMachineGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(this->GetStateName());
}

FString UStateMachineGraphNode::GetStateName() const
{
	return (this->State) ? this->State->RuntimeData.Name : "";
}

void UStateMachineGraphNode::ReallocateDefaultPins()
{
	// voluntary empty
}

void UStateMachineGraphNode::GetTransitionList(TArray<class UStateMachineGraphNode_Transition*>& OutTransitions, bool bWantSortedList /*= false*/)
{
	// Normal transitions
	for (int32 LinkIndex = 0; LinkIndex < Pins[1]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[1]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UStateMachineGraphNode_Transition* Transition = Cast<UStateMachineGraphNode_Transition>(TargetNode))
			OutTransitions.Add(Transition);
	}

	// Bidirectional transitions where we are the 'backwards' link
	for (int32 LinkIndex = 0; LinkIndex < Pins[0]->LinkedTo.Num(); ++LinkIndex)
	{
		UEdGraphNode* TargetNode = Pins[0]->LinkedTo[LinkIndex]->GetOwningNode();
		if (UStateMachineGraphNode_Transition* Transition = Cast<UStateMachineGraphNode_Transition>(TargetNode))
			OutTransitions.Add(Transition);
	}

	// Sort the transitions by priority order, lower numbers are higher priority
	if (bWantSortedList)
	{
		struct FCompareTransitionsByPriority
		{
			FORCEINLINE bool operator()(const UStateMachineGraphNode_Transition& A, const UStateMachineGraphNode_Transition& B) const
			{
				return A.PriorityOrder < B.PriorityOrder;
			}
		};

		OutTransitions.Sort(FCompareTransitionsByPriority());
	}
}

void UStateMachineGraphNode::CreateBoundGraph()
{
 	if (UStateMachineGraph* Graph = Cast<UStateMachineGraph>(GetGraph()))
 	{
		if (UStateMachineBlueprint* StateMachineBlueprint = Graph->StateMachineBlueprint)
			FStateMachineUtilities::AddNewBoundedGraphToState(Graph, this);
	}
}
