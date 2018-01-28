/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_Restart.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_Restart : public UNpcBehaviorTask_Default
{
	GENERATED_BODY()

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Execute() override; 
};
