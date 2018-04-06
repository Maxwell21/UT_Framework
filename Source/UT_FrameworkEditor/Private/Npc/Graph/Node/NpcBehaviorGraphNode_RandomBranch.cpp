/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphNode_RandomBranch.h"
#include "NpcBehaviorTask_RandomBranch.h"
#include "NpcBehaviorTask_Default.h"
#include "EdGraph/EdGraphPin.h"
#include "FrameworkGenerics.h"

UNpcBehaviorGraphNode_RandomBranch::UNpcBehaviorGraphNode_RandomBranch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNpcBehaviorGraphNode_RandomBranch::ReconstructNode()
{
	this->InputPin = this->Pins[0];
	this->RefreshAllOutputPins();

	FGenericNodeData NodeData = this->Task->NodeData;
	this->NodePosX = NodeData.NodePosX;
	this->NodePosY = NodeData.NodePosY;
}

FText UNpcBehaviorGraphNode_RandomBranch::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("RandomBranch");
}

void UNpcBehaviorGraphNode_RandomBranch::AllocateDefaultPins()
{
	check(Pins.Num() == 0);

	InputPin  = CreatePin(EGPD_Input, TEXT("RandomBranch"), FName(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_RandomBranch::GetNodeTitleColor() const
{
	return FLinearColor(0.254706f, 0.187059f, 0.44902f);
}

void UNpcBehaviorGraphNode_RandomBranch::RefreshAllOutputPins()
{
	this->CleanAllOutputsPin();

	if (UNpcBehaviorTask_RandomBranch* RandomBranchTask = Cast<UNpcBehaviorTask_RandomBranch>(this->Task))
	{
		for (int32 Index = 0; Index < RandomBranchTask->Number; ++Index)
		{
			if (RandomBranchTask->MultipleTargets.IsValidIndex(Index) == false)
			{
				UEdGraphPin* NewPin = CreatePin(EGPD_Output, TEXT("RandomBranch"), FName(), nullptr, TEXT(""));
				this->MultipleOutputPins.AddUnique(NewPin);

				FNpcBehaviorTask_MultipleTarget Target;
				RandomBranchTask->MultipleTargets.Add(Target);
			}
		}
	}
}

void UNpcBehaviorGraphNode_RandomBranch::OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	if (UNpcBehaviorTask_RandomBranch* RandomBranchTask = Cast<UNpcBehaviorTask_RandomBranch>(this->Task))
	{
		// ---
	}
}

TArray<UNpcBehaviorTask_Default*> UNpcBehaviorGraphNode_RandomBranch::GetAllNextTasks()
{
	TArray<UNpcBehaviorTask_Default*> Tasks;

	for (FNpcBehaviorTask_MultipleTarget Target : Cast<UNpcBehaviorTask_RandomBranch>(this->Task)->MultipleTargets)
	{
		Tasks.Add(Target.NextTask);
	}

	return Tasks;
}
