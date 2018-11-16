/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "K2Node_BaseAsyncTask.h"
#include "K2Node_ConstructStateMachine.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UK2Node_ConstructStateMachine : public UK2Node_BaseAsyncTask
{
	GENERATED_UCLASS_BODY()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	//~ Begin UEdGraphNode Interface
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UK2Node Interface
	virtual FText GetMenuCategory() const override;
	//~ End UK2Node Interface


};
