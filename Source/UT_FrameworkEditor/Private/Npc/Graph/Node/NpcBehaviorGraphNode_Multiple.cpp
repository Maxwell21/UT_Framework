/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_Multiple.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_Multiple::UNpcBehaviorGraphNode_Multiple(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UNpcBehaviorGraphNode_Multiple::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Multiple");
}

void UNpcBehaviorGraphNode_Multiple::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin  = CreatePin(EGPD_Input, TEXT("Multiple"), FName(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_Multiple::GetNodeTitleColor() const
{
	return FLinearColor(0.164706f, 0.447059f, 0.14902f);
}

void UNpcBehaviorGraphNode_Multiple::CleanAllOutputsPin()
{
	if (Pins.Num() > 0)
	{
		for (auto PinToRemove : this->Pins)
		{
			if (PinToRemove->Direction == EGPD_Output)
				RemovePin(PinToRemove);
		}
	}
}

void UNpcBehaviorGraphNode_Multiple::RefreshAllOutputPins()
{
	// ---
}

void UNpcBehaviorGraphNode_Multiple::OnNotifyGraphChanged()
{
	this->RefreshAllOutputPins();
	Super::OnNotifyGraphChanged();
}

TArray<UNpcBehaviorTask_Default*> UNpcBehaviorGraphNode_Multiple::GetAllNextTasks()
{
	return TArray<UNpcBehaviorTask_Default*>();
}
