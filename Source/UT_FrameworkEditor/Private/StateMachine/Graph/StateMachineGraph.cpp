/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraph.h"
#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Entry.h"
#include "StateMachineGraphNode_Transition.h"
#include "StateMachineState_Default.h"
#include "StateMachineState_Entry.h"
#include "StateMachineTransition_Default.h"
#include "StateMachineBlueprint.h"
#include "StateMachineUtilities.h"
#include "StateMachine.h"
#include "Package.h"
#include "EdGraph/EdGraphPin.h"

void UStateMachineGraph::Init(UStateMachineBlueprint* InStateMachineBlueprint)
{
	this->StateMachineBlueprint = InStateMachineBlueprint;
	this->RebuildGraph();
}

void UStateMachineGraph::RebuildGraph()
{
	// Don't allow initial graph rebuild to affect package dirty state; remember current state...
	UPackage* Package = GetOutermost();
	const bool bIsDirty = Package->IsDirty();

	Modify();

	this->RemoveAllNodes();
	this->InitDefaultNode();
	this->ReconstructNodes();
	this->LinkStates();

	NotifyGraphChanged();

	// ...and restore it
	Package->SetDirtyFlag(bIsDirty);
}

void UStateMachineGraph::ReconstructNodes()
{
	// Reconstruct nodes
	for (UEdGraphNode* EdGraphNode : this->StateMachineBlueprint->Nodes)
	{
		if (UStateMachineGraphNode_Transition* TransitionStateMachineNode = Cast<UStateMachineGraphNode_Transition>(EdGraphNode))
		{
			if (!TransitionStateMachineNode->Transition)
				continue;

			TransitionStateMachineNode->Transition->Modify();

			TransitionStateMachineNode->ReconstructNode();
			TransitionStateMachineNode->Transition->GraphNode = TransitionStateMachineNode;
			this->AddNode(TransitionStateMachineNode, false, false);

			// Apply current blueprint object edited
			this->SetCurrentStateMachineBlueprintToNode(TransitionStateMachineNode);

			TransitionStateMachineNode->Transition->PostEditChange();
			TransitionStateMachineNode->Transition->MarkPackageDirty();
		}
		else if (UStateMachineGraphNode_Entry* EntryStateMachineNode = Cast<UStateMachineGraphNode_Entry>(EdGraphNode))
		{
			EntryStateMachineNode->ReconstructNode();
			this->AddNode(EntryStateMachineNode, false, false);

			// Apply current blueprint object edited
			this->SetCurrentStateMachineBlueprintToNode(EntryStateMachineNode);
		}
		else if (UStateMachineGraphNode* StateMachineNode = Cast<UStateMachineGraphNode>(EdGraphNode))
		{
			if (!StateMachineNode->State)
				continue;

			StateMachineNode->State->Modify();

			StateMachineNode->ReconstructNode();
			StateMachineNode->State->GraphNode = StateMachineNode;
			this->AddNode(StateMachineNode, false, false);

			// Apply current blueprint object edited
			this->SetCurrentStateMachineBlueprintToNode(StateMachineNode);

			StateMachineNode->State->PostEditChange();
			StateMachineNode->State->MarkPackageDirty();
		}
	}
}

void UStateMachineGraph::RegisterNode(UStateMachineState_Default* State, UStateMachineGraphNode* NewNode, UEdGraphPin* FromPin)
{
	Modify();
	this->StateMachineBlueprint->Modify();

	//Register node to the nodes array
	this->StateMachineBlueprint->Nodes.Add(NewNode);
	this->StateMachineBlueprint->GetStateMachine()->GetGeneratedClass()->States.Add(State);

	State->GraphNode = NewNode;
	NewNode->State = State;
	NewNode->ReallocateDefaultPins();

	// Previous node 
	if (FromPin)
	{
		UStateMachineGraphNode* FromNode = Cast<UStateMachineGraphNode>(FromPin->GetOwningNode());

		// apply current blueprint edited
		this->SetCurrentStateMachineBlueprintToNode(FromNode);

		if (NewNode->GetRootNode() == false)
			NewNode->CreateBoundGraph();

		if (FromNode->GetRootNode())
			this->SetEntryState(State);
	}

	this->SetCurrentStateMachineBlueprintToNode(NewNode);

	// linking between pins
	NewNode->AutowireNewNode(FromPin);

	NotifyGraphChanged();
}

void UStateMachineGraph::RegisterEntryNode(UStateMachineState_Entry* Entry, UStateMachineGraphNode* NewNode)
{
	Modify();
	this->StateMachineBlueprint->Modify();

	//Register node to the nodes array
	this->StateMachineBlueprint->Nodes.Add(NewNode);
	this->StateMachineBlueprint->EntryNode = NewNode;
	this->StateMachineBlueprint->GetStateMachine()->GetGeneratedClass()->EntryState = Entry;

	Entry->GraphNode = NewNode;
	NewNode->ReallocateDefaultPins();

	this->SetCurrentStateMachineBlueprintToNode(NewNode);

	NotifyGraphChanged();
}

void UStateMachineGraph::RegisterTransitionNode(UStateMachineTransition_Default* Transition, UStateMachineGraphNode_Transition* NewNode)
{
	Modify();
	this->StateMachineBlueprint->Modify();

	//Register node to the nodes array
	this->StateMachineBlueprint->Nodes.Add(NewNode);

	Transition->GraphNode = NewNode;
	NewNode->Transition = Transition;

	this->SetCurrentStateMachineBlueprintToNode(NewNode);
	NotifyGraphChanged();
}

void UStateMachineGraph::LinkStates()
{
	// Link entry node
	if (this->EntryNode->State)
	{
		if (UStateMachineGraphNode* LinkedNode = this->FindNodeByState(this->EntryNode->State))
			this->EntryNode->GetOutputPin()->MakeLinkTo(LinkedNode->GetInputPin());
	}

	// Link nodes
	for (UEdGraphNode* EdGraphNode : this->StateMachineBlueprint->Nodes)
	{
		if (UStateMachineGraphNode* Node = Cast<UStateMachineGraphNode>(EdGraphNode))
		{
			if (!Node->State || Node->GetRootNode())
				continue;

			Node->State->Modify();

			if (Node->State->Transitions.Num() > 0)
			{
				for (UStateMachineTransition_Default* Transition : Node->State->Transitions)
				{
					if (UStateMachineGraphNode_Transition* TransNode = this->FindNodeByTransition(Transition))
						TransNode->LinkNode(this->FindNodeByState(Transition->FromState), this->FindNodeByState(Transition->ToState));
				}
			}

			Node->State->PostEditChange();
			Node->State->MarkPackageDirty();
		}
	}
}

void UStateMachineGraph::InitDefaultNode()
{
	if (this->StateMachineBlueprint->EntryNode)
	{
		this->EntryNode = Cast<UStateMachineGraphNode_Entry>(this->StateMachineBlueprint->EntryNode);
		return;
	}

	// Create the entry 
	this->EntryNode = FStateMachineUtilities::AddNewEntry(0, 0, this);
}

UObject* UStateMachineGraph::PrepareOuter()
{
	return this->StateMachineBlueprint;
}

void UStateMachineGraph::SetEntryState(UStateMachineState_Default* State)
{
	if (this->EntryNode)
	{
		this->EntryNode->State = State;
		UStateMachine* StateMachine = this->StateMachineBlueprint->GetStateMachine();
		StateMachine->GetGeneratedClass()->EntryState->EntryState = State;
		StateMachine->SetRootState(State->RuntimeData.Name);
	}
}

void UStateMachineGraph::RemoveAllNodes()
{
	TArray<UEdGraphNode*> NodesToRemove = Nodes;
	for (int32 NodeIndex = 0; NodeIndex < NodesToRemove.Num(); ++NodeIndex)
	{
		// Reset graph node of State object
		if (UStateMachineGraphNode* NodeToRemove = Cast<UStateMachineGraphNode>(NodesToRemove[NodeIndex]))
		{
			if (NodeToRemove->State)
				NodeToRemove->State->GraphNode = nullptr;
		}

		NodesToRemove[NodeIndex]->Modify();
		RemoveNode(NodesToRemove[NodeIndex]);
	}
}

void UStateMachineGraph::SetCurrentStateMachineBlueprintToNode(UStateMachineGraphNode* Node)
{
	if (Node && Node->State)
		Node->State->EditedStateMachineBlueprint = this->StateMachineBlueprint;
	else if (UStateMachineGraphNode_Transition* TransNode = Cast<UStateMachineGraphNode_Transition>(Node))
		TransNode->Transition->EditedStateMachineBlueprint = this->StateMachineBlueprint;
}

UStateMachineGraphNode* UStateMachineGraph::FindNodeByState(UStateMachineState_Default* State)
{
	for (UEdGraphNode* Node : this->Nodes)
	{
		UStateMachineGraphNode* StateNode = Cast<UStateMachineGraphNode>(Node);
		if (StateNode && StateNode->State && StateNode->State == State && StateNode->GetRootNode() == false)
			return StateNode;
	}

	return nullptr;
}

UStateMachineGraphNode_Transition* UStateMachineGraph::FindNodeByTransition(UStateMachineTransition_Default* Transition)
{
	for (UEdGraphNode* Node : this->Nodes)
	{
		UStateMachineGraphNode_Transition* TransitionNode = Cast<UStateMachineGraphNode_Transition>(Node);
		if (TransitionNode && TransitionNode->Transition && TransitionNode->Transition == Transition)
			return TransitionNode;
	}

	return nullptr;
}

void UStateMachineGraph::NotifyNodesGraphChanged()
{
	for (auto const& Node : this->Nodes)
	{
		CastChecked<UStateMachineGraphNode>(Node)->OnNotifyGraphChanged();
	}
}
