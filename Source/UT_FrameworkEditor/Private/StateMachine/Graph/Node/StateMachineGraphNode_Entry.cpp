/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphNode_Entry.h"
#include "StateMachineGraphNode.h"
#include "StateMachineState_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UStateMachineGraphNode_Entry::UStateMachineGraphNode_Entry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->IsRootNode = true;
	this->bCanRenameNode = false;
}

void UStateMachineGraphNode_Entry::ReconstructNode()
{
	this->OutputPin = this->Pins[0];
}

FText UStateMachineGraphNode_Entry::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Entry");
}

void UStateMachineGraphNode_Entry::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	OutputPin = CreatePin(EGPD_Output, TEXT("Entry"), FName(), nullptr, TEXT(""));
}

void UStateMachineGraphNode_Entry::CreateBoundGraph()
{

}

