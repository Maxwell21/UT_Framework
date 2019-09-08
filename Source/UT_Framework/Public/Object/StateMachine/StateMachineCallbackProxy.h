/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "StateMachineCallbackProxy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConstructStateMachineDelegate, UStateMachine*, StateMachine);

class UStateMachine;
class APlayerController;

/**
 * 
 */
UCLASS(MinimalAPI)
class UStateMachineCallbackProxy : public UObject
{
	GENERATED_UCLASS_BODY()

	/************************************************************************/
	/* DELEGATES                                                            */
	/************************************************************************/

	UPROPERTY(BlueprintAssignable)
	FOnConstructStateMachineDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnConstructStateMachineDelegate OnFailed;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// Called to perform the query internally
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UStateMachineCallbackProxy* CreateProxyObjectForConstructStateMachine(
		UObject* WorldContextObject,
		TSubclassOf<UStateMachine> Template,
		UObject* Owner,
		APlayerController* Controller);

};
