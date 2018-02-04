/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Entry.generated.h"

class UStateMachineState_Default;

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UStateMachineGraphNode_Entry : public UStateMachineGraphNode
{
	GENERATED_UCLASS_BODY()

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UEdGraphNode interface.
	virtual void ReconstructNode() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	// End UEdGraphNode interface.

	virtual void CreateBoundGraph() override;
	
};
