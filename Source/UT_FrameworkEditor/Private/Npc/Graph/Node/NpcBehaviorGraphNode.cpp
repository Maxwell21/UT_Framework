/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorGraph.h"
#include "NpcBehaviorGraphSchema.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorBlueprint.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode::UNpcBehaviorGraphNode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	Task(nullptr),
	IsRootNode(false),
	InputPin(nullptr),
	OutputPin(nullptr)
{
}

void UNpcBehaviorGraphNode::SetIsRootNode(bool IsNewRootNode)
{
	this->IsRootNode = IsNewRootNode;
}

void UNpcBehaviorGraphNode::ReconstructNode()
{
	this->InputPin = this->Pins[0];
	this->OutputPin = this->Pins[1];

	FGenericNodeData NodeData = this->Task->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

bool UNpcBehaviorGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema->IsA(UNpcBehaviorGraphSchema::StaticClass());
}

bool UNpcBehaviorGraphNode::CanUserDeleteNode() const
{
	return !this->IsRootNode;
}

void UNpcBehaviorGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	UNpcBehaviorGraph* Graph = Cast<UNpcBehaviorGraph>(GetGraph());
	if (Graph && FromPin)
	{
		if (const UNpcBehaviorGraphSchema* Schema = Cast<UNpcBehaviorGraphSchema>(Graph->GetSchema()))
		{
			if (FromPin->Direction == EGPD_Output)
			{
				Schema->TryCreateConnection(FromPin, this->InputPin);
				Graph->SetNextTask(FromPin, this);
			}
			else
			{
				Schema->TryCreateConnection(this->OutputPin, FromPin);
				Graph->SetNextTask(FromPin, this);
			}
		}
	}
}

void UNpcBehaviorGraphNode::ResetLinkedTask(UEdGraphPin* Pin/* = nullptr*/)
{
	this->Task->NextTask = nullptr;
}

void UNpcBehaviorGraphNode::DestroyNode()
{
	// Remove the next task relation between the from node to this node
	if (this->InputPin && this->InputPin->LinkedTo.Num() > 0)
	{
		auto FromNode = Cast<UNpcBehaviorGraphNode>(this->InputPin->LinkedTo[0]->GetOwningNode());

		UNpcBehaviorGraphNode* Node = Cast<UNpcBehaviorGraphNode>(FromNode);
		if (Node && Node->GetRootNode())
			CastChecked<UNpcBehaviorGraph>(GetGraph())->NpcBehaviorBlueprint->EntryNode = nullptr;
		else if (FromNode)
			FromNode->Task->NextTask = nullptr;
	}

	CastChecked<UNpcBehaviorGraph>(GetGraph())->NpcBehaviorBlueprint->Nodes.Remove(this);

	Super::DestroyNode();
}

void UNpcBehaviorGraphNode::SaveNode()
{
	if (this->Task)
	{
		FGenericNodeData NodeData;
		NodeData.NodePosX = this->NodePosX;
		NodeData.NodePosY = this->NodePosY;
		this->Task->NodeData = NodeData;
	}
}

void UNpcBehaviorGraphNode::OnNotifyGraphChanged()
{
	this->SaveNode();
}

void UNpcBehaviorGraphNode::OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{

}

FLinearColor UNpcBehaviorGraphNode::GetNodeTitleColor() const
{
	return FLinearColor(0.609375f, 0.038051f, 0.044112f);
}

void UNpcBehaviorGraphNode::ReallocateDefaultPins()
{
	// volumptary empty
}
