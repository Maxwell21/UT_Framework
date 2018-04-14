/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_Wait.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_Wait::UNpcBehaviorGraphNode_Wait(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UNpcBehaviorGraphNode_Wait::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Wait");
}

void UNpcBehaviorGraphNode_Wait::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin  = CreatePin(EGPD_Input, TEXT("Wait"), FName(), nullptr, TEXT(""));
	OutputPin = CreatePin(EGPD_Output, TEXT("Wait"), FName(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_Wait::GetNodeTitleColor() const
{
	return FLinearColor(0.164706f, 0.447059f, 0.14902f);
}
