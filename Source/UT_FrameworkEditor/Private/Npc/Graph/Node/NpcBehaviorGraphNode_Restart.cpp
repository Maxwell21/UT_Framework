/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_Restart.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_Restart::UNpcBehaviorGraphNode_Restart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNpcBehaviorGraphNode_Restart::ReconstructNode()
{
	this->InputPin = this->Pins[0];

	FGenericNodeData NodeData = this->Task->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

FText UNpcBehaviorGraphNode_Restart::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Restart");
}

void UNpcBehaviorGraphNode_Restart::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin  = CreatePin(EGPD_Input, TEXT("Restart"), FName(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_Restart::GetNodeTitleColor() const
{
	return FLinearColor(0.212796f, 0.491802f, 0.651042f);
}
