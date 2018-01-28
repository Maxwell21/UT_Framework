/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_Multiple.generated.h"


USTRUCT()
struct FNpcBehaviorTask_MultipleTarget
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	UNpcBehaviorTask_Default* NextTask;
	
};

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_Multiple : public UNpcBehaviorTask_Default
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/
	
	UPROPERTY()
	TArray<FNpcBehaviorTask_MultipleTarget> MultipleTargets;
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

};
