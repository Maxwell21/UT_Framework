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
	UEdGraphNode::ReconstructNode();
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

	InputPin  = CreatePin(EGPD_Input, TEXT("RandomBranch"), FString(), nullptr, TEXT(""));
}

FLinearColor UNpcBehaviorGraphNode_RandomBranch::GetNodeTitleColor() const
{
	return FLinearColor(0.254706f, 0.187059f, 0.44902f);
}

void UNpcBehaviorGraphNode_RandomBranch::RefreshAllOutputPins()
{
	if (UNpcBehaviorTask_RandomBranch* RandomBranchTask = Cast<UNpcBehaviorTask_RandomBranch>(this->Task))
	{
		int32 Index = 0;
		for (Index; Index < RandomBranchTask->Number; Index++)
		{
			if (RandomBranchTask->MultipleTargets.IsValidIndex(Index) == false)
			{
				UEdGraphPin* NewPin = CreatePin(EGPD_Output, TEXT("RandomBranch"), FString(), nullptr, TEXT(""));
				this->MultipleOutputPins.Add(NewPin);

				FNpcBehaviorTask_MultipleTarget Target;
				Target.NextTask = nullptr;
				RandomBranchTask->MultipleTargets.Add(Target);
			}

			if (this->MultipleOutputPins.IsValidIndex(Index) == false)
			{
				UEdGraphPin* NewPin = CreatePin(EGPD_Output, TEXT("RandomBranch"), FString(), nullptr, TEXT(""));
				this->MultipleOutputPins.Add(NewPin);
			}
		}

		int32 Count = this->MultipleOutputPins.Num();
		for (Count; Count > Index; Count--)
		{
			// Remove only output pins
			if (this->MultipleOutputPins.IsValidIndex(Count-1))
			{
				RandomBranchTask->MultipleTargets.RemoveAt(Count-1);
				this->MultipleOutputPins.RemoveAt(Count-1);
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
