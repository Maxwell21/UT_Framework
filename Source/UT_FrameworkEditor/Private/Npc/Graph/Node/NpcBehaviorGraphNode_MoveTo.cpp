/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_MoveTo.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_MoveTo::UNpcBehaviorGraphNode_MoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UNpcBehaviorGraphNode_MoveTo::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("MoveTo");
}

void UNpcBehaviorGraphNode_MoveTo::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin  = CreatePin(EGPD_Input, TEXT("MoveTo"), FString(), nullptr, TEXT(""));
	OutputPin = CreatePin(EGPD_Output, TEXT("MoveTo"), FString(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_MoveTo::GetNodeTitleColor() const
{
	return FLinearColor(0.13693f, 0.05089f, 0.348958f);
}
