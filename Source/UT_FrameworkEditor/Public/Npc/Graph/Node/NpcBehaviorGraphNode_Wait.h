/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "NpcBehaviorGraphNode.h"
#include "NpcBehaviorGraphNode_Wait.generated.h"

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UNpcBehaviorGraphNode_Wait : public UNpcBehaviorGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UEdGraphNode interface.
	virtual	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual	void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface.

};
