/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "NpcBehaviorGraphNode_Multiple.h"
#include "NpcBehaviorGraphNode_RandomBranch.generated.h"

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UNpcBehaviorGraphNode_RandomBranch : public UNpcBehaviorGraphNode_Multiple
{
	GENERATED_UCLASS_BODY()

public:
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UEdGraphNode interface.
	virtual void ReconstructNode() override;
	virtual	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual	void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface.

	virtual void RefreshAllOutputPins() override;
	virtual void OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;
	virtual TArray<UNpcBehaviorTask_Default*> GetAllNextTasks() override;
};
