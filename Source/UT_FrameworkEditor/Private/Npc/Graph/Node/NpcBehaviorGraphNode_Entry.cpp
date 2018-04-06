/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_Entry.h"
#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_Entry::UNpcBehaviorGraphNode_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNpcBehaviorGraphNode_Entry::ReconstructNode()
{
	this->OutputPin = this->Pins[0];

	FGenericNodeData NodeData = this->Task->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

FText UNpcBehaviorGraphNode_Entry::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Entry");
}

void UNpcBehaviorGraphNode_Entry::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	OutputPin = CreatePin(EGPD_Output, TEXT("Entry"), FName(), nullptr, TEXT(""));
}

