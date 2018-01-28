/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_Wait.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_Wait : public UNpcBehaviorTask_Default
{
	GENERATED_BODY()

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "Npc Wait", EditDefaultsOnly)
	float Time = 2.f;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Execute() override; 
};
