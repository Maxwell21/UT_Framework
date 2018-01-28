/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Multiple.h"
#include "NpcBehaviorTask_RandomBranch.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_RandomBranch : public UNpcBehaviorTask_Multiple
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "Npc RandomBranch", EditDefaultsOnly)
	int32 Number;
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Execute() override; 
};
