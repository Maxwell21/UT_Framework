/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "StateMachineBlueprintGeneratedClass.generated.h"

class UStateMachineState_Entry;
class UStateMachineState_Default;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_UCLASS_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	UStateMachineState_Entry* EntryState;

	UStateMachineState_Default* CurrentState = nullptr;

	UPROPERTY()
	TArray<UStateMachineState_Default*> States;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UObject interface
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
	virtual void Serialize(FArchive& Ar) override;
	virtual UObject* CreateDefaultObject() override;
	virtual void PostLoad() override;
	virtual bool NeedsLoadForServer() const override;
	// End UObject interface

	virtual void PurgeClass(bool bRecompilingOnLoad) override;
};
