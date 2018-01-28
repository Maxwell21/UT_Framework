/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_Entry.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_Entry : public UNpcBehaviorTask_Default
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Execute() override;
};
