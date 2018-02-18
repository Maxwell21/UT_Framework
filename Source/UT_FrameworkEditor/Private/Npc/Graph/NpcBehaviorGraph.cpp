/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraph.h"
#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorGraphNode_Multiple.h"
#include "NpcBehaviorGraphNode_Entry.h"
#include "NpcBehaviorUtilities.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_Entry.h"
#include "NpcBehaviorTask_Multiple.h"
#include "NpcBehaviorBlueprint.h"
#include "NpcBehavior.h"
#include "Package.h"
#include "EdGraph/EdGraphPin.h"

void UNpcBehaviorGraph::Init(UNpcBehaviorBlueprint* InNpcBehaviorBlueprint)
{
	this->NpcBehaviorBlueprint = InNpcBehaviorBlueprint;
	this->RebuildGraph();
}

void UNpcBehaviorGraph::RebuildGraph()
{
 	// Don't allow initial graph rebuild to affect package dirty state; remember current state...
	UPackage* Package = GetOutermost();
	const bool bIsDirty = Package->IsDirty();

	Modify();

 	this->RemoveAllNodes();
 	this->InitDefaultNode();
	this->ReconstructNodes();
 	this->LinkTasks();

	NotifyGraphChanged();

 	// ...and restore it
	Package->SetDirtyFlag(bIsDirty);
}

void UNpcBehaviorGraph::ReconstructNodes()
{
	// Reconstruct nodes
	for (UEdGraphNode* EdGraphNode : this->NpcBehaviorBlueprint->Nodes)
	{
		if (UNpcBehaviorGraphNode* NpcBehaviorNode = Cast<UNpcBehaviorGraphNode>(EdGraphNode))
		{
			if (!NpcBehaviorNode->Task)
				continue;

			NpcBehaviorNode->Task->Modify();

			NpcBehaviorNode->ReconstructNode();
			this->AddNode(NpcBehaviorNode, false, false);

			// Apply current blueprint object edited
			this->SetCurrentNpcBlueprintToNode(NpcBehaviorNode);

			NpcBehaviorNode->Task->PostEditChange();
			NpcBehaviorNode->Task->MarkPackageDirty();
		}
	}
}

void UNpcBehaviorGraph::RegisterNode(UNpcBehaviorTask_Default* Task, UNpcBehaviorGraphNode* NewNode, UEdGraphPin* FromPin)
{
	Modify();
	this->NpcBehaviorBlueprint->Modify();

	//Register node to the npc nodes array
	this->NpcBehaviorBlueprint->Nodes.Add(NewNode);

	Task->GraphNode = NewNode;
	NewNode->Task = Task;
	NewNode->ReallocateDefaultPins();

	// Previous node 
	if (FromPin)
	{
		UNpcBehaviorGraphNode* FromNode = Cast<UNpcBehaviorGraphNode>(FromPin->GetOwningNode());

		// apply current blueprint edited
		this->SetCurrentNpcBlueprintToNode(FromNode);

		// Re-link task
		if (!FromNode->GetRootNode())
			this->SetNextTask(FromPin, NewNode);
		else
			this->CheckEntryNodeIsSet(FromNode, NewNode);
	}

	this->SetCurrentNpcBlueprintToNode(NewNode);

	// linking between pins
	NewNode->AutowireNewNode(FromPin);

	NotifyGraphChanged();
}

void UNpcBehaviorGraph::LinkTasks()
{
	// Link entry node
	if (UNpcBehaviorGraphNode* LinkedNode = this->FindNodeByTask(this->NpcBehaviorBlueprint->GetNpcBehavior()->EntryTask->NextTask))
		this->EntryNode->GetOutputPin()->MakeLinkTo(LinkedNode->GetInputPin());

	// Link nodes
	for (UEdGraphNode* EdGraphNode : this->NpcBehaviorBlueprint->Nodes)
	{
		//Check if it's a simple node or a multiple node
		if (UNpcBehaviorGraphNode_Multiple* NpcBehaviorMultipleNode = Cast<UNpcBehaviorGraphNode_Multiple>(EdGraphNode))
		{
			TArray<UNpcBehaviorTask_Default*> NextTasks = NpcBehaviorMultipleNode->GetAllNextTasks();
			// check if we have the same size between outputs pins and next nodes
			if (!NpcBehaviorMultipleNode->Task && NpcBehaviorMultipleNode->MultipleOutputPins.Num() != NextTasks.Num())
				continue;

			NpcBehaviorMultipleNode->Task->Modify();

			for (int32 Index = 0; Index < NextTasks.Num(); ++Index)
			{
				// Not linked
				if (NextTasks[Index] == nullptr)
					continue;

				if (UNpcBehaviorGraphNode* LinkedNode = this->FindNodeByTask(NextTasks[Index]))
					NpcBehaviorMultipleNode->MultipleOutputPins[Index]->MakeLinkTo(LinkedNode->GetInputPin());
			}

			NpcBehaviorMultipleNode->Task->PostEditChange();
			NpcBehaviorMultipleNode->Task->MarkPackageDirty();
		}
		if (UNpcBehaviorGraphNode* NpcBehaviorNode = Cast<UNpcBehaviorGraphNode>(EdGraphNode))
		{
			if (!NpcBehaviorNode->Task && !NpcBehaviorNode->Task->NextTask || NpcBehaviorNode->GetRootNode())
				continue;

			NpcBehaviorNode->Task->Modify();

			if (UNpcBehaviorGraphNode* LinkedNode = this->FindNodeByTask(NpcBehaviorNode->Task->NextTask))
				NpcBehaviorNode->GetOutputPin()->MakeLinkTo(LinkedNode->GetInputPin());

			NpcBehaviorNode->Task->PostEditChange();
			NpcBehaviorNode->Task->MarkPackageDirty();
		}
	}
}

void UNpcBehaviorGraph::InitDefaultNode()
{
	if (this->NpcBehaviorBlueprint->EntryNode)
	{
		this->EntryNode = Cast<UNpcBehaviorGraphNode_Entry>(this->NpcBehaviorBlueprint->EntryNode);
		return;
	}
	// Create the entry 
	this->EntryNode = FNpcBehaviorUtilities::AddNewTask<UNpcBehaviorGraphNode_Entry, UNpcBehaviorTask_Entry>(0, 0, this);
	this->EntryNode->SetIsRootNode(true);
	this->NpcBehaviorBlueprint->EntryNode = this->EntryNode;
	this->NpcBehaviorBlueprint->GetNpcBehavior()->EntryTask = Cast<UNpcBehaviorTask_Entry>(this->EntryNode->Task);
}

void UNpcBehaviorGraph::CheckEntryNodeIsSet(UNpcBehaviorGraphNode* FromNode, UNpcBehaviorGraphNode* CurrentNode)
{
	if (FromNode->GetRootNode() && this->NpcBehaviorBlueprint->EntryNode == nullptr)
	{
		FGenericNodeData NodeData;
		NodeData.NodePosX = CurrentNode->NodePosX;
		NodeData.NodePosY = CurrentNode->NodePosY;
		this->NpcBehaviorBlueprint->Modify();
		this->NpcBehaviorBlueprint->GetNpcBehavior()->EntryTask->NextTask = CurrentNode->Task;
	}
}

UObject* UNpcBehaviorGraph::PrepareOuter()
{
	return this->NpcBehaviorBlueprint->GetNpcBehavior();
}

void UNpcBehaviorGraph::SetNextTask(UEdGraphPin* FromPin, UNpcBehaviorGraphNode* ToNode)
{
	UNpcBehaviorGraphNode* FromNode = Cast<UNpcBehaviorGraphNode>(FromPin->GetOwningNode());
	
	if (!FromPin || !FromNode || !ToNode)
		return;

	if (Cast<UNpcBehaviorGraphNode_Multiple>(FromNode) && Cast<UNpcBehaviorTask_Multiple>(FromNode->Task))
	{
		UNpcBehaviorTask_Multiple* MultipleTask = Cast<UNpcBehaviorTask_Multiple>(FromNode->Task);
		// minus one because MultipleTargets don't take account of InputPin 
		int32 CurrentPinIndex = FromNode->GetPinIndex(FromPin) - 1;
		for (int32 Index = 0; Index < MultipleTask->MultipleTargets.Num(); Index++)
		{
 			if (Index == CurrentPinIndex && MultipleTask->MultipleTargets.IsValidIndex(Index))
 			{
				MultipleTask->MultipleTargets[Index].NextTask = ToNode->Task;
				break;
			}
		}
	}
	else if (ToNode->Task)
		FromNode->Task->NextTask = ToNode->Task;
}

void UNpcBehaviorGraph::RemoveAllNodes()
{
	TArray<UEdGraphNode*> NodesToRemove = Nodes;
	for (int32 NodeIndex = 0; NodeIndex < NodesToRemove.Num(); ++NodeIndex)
	{
		// Reset graph node of task object
		if (UNpcBehaviorGraphNode* NodeToRemove = Cast<UNpcBehaviorGraphNode>(NodesToRemove[NodeIndex]))
		{
			if (NodeToRemove->Task)
				NodeToRemove->Task->GraphNode = nullptr;
		}

		NodesToRemove[NodeIndex]->Modify();
		RemoveNode(NodesToRemove[NodeIndex]);
	}
}
 
void UNpcBehaviorGraph::SetCurrentNpcBlueprintToNode(UNpcBehaviorGraphNode* Node)
{
	if (Node && Node->Task)
		Node->Task->EditedNpcBlueprint = this->NpcBehaviorBlueprint;
}

UNpcBehaviorGraphNode* UNpcBehaviorGraph::FindNodeByTask(UNpcBehaviorTask_Default* Task)
{
	for (UEdGraphNode* Node : this->Nodes)
	{
		UNpcBehaviorGraphNode* TaskNode = Cast<UNpcBehaviorGraphNode>(Node);
		if (TaskNode && TaskNode->Task && TaskNode->Task == Task)
			return TaskNode;
	}

	return nullptr;
}

void UNpcBehaviorGraph::NotifyNodesGraphChanged()
{
	for (auto const& Node : this->Nodes)
	{
		CastChecked<UNpcBehaviorGraphNode>(Node)->OnNotifyGraphChanged();
	}
}
