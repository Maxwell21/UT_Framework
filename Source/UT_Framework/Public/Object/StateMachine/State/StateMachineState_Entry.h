/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateMachineState_Default.h"
#include "StateMachineState_Entry.generated.h"

class UStateMachineState_Default;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UStateMachineState_Entry : public UStateMachineState_Default
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	UStateMachineState_Default* State;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
};
