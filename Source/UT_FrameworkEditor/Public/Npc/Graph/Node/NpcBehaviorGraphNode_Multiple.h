/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorGraphNode_Multiple.generated.h"

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UNpcBehaviorGraphNode_Multiple : public UNpcBehaviorGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	
	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	TArray<UEdGraphPin*> MultipleOutputPins;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UEdGraphNode interface.
	virtual	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual	void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface.

	virtual void CleanAllOutputsPin();
	virtual void RefreshAllOutputPins();
	virtual void OnNotifyGraphChanged() override;
	virtual TArray<UNpcBehaviorTask_Default*> GetAllNextTasks();
};
