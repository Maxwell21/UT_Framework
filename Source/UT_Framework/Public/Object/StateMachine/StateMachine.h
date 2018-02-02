/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateMachine.generated.h"

class UStateMachineState_Default;
class UStateMachineState_Entry;
class UStateMachineBlueprint;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UT_FRAMEWORK_API UStateMachine : public UObject
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	UStateMachineState_Entry* EntryState;

	UStateMachineState_Default* CurrentState = nullptr;

	UPROPERTY()
	TArray<UStateMachineState_Default*> States;

	UPROPERTY()
	UObject* OwnerObject;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Construct a new state machine from a template
	*
	* @params TSubclassOf<UStateMachineBlueprint> Template
	* @params UObject* Owner
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	static UStateMachine* ConstructStateMachine(UStateMachine* Template, UObject* Owner);

	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	static UStateMachine* CreateStateMachine(UStateMachineBlueprint* Template, UObject* Owner);

	/**
	 * Initialize state machine
	 *
	 * @params UObject* Owner
	 */
	virtual void Init(UObject* Owner);

	/**
	* Start state machine
	*/
	UFUNCTION(Category="Umbra Framework | State Machine", BlueprintCallable)
	virtual void Start();

	/**
	 * Get the current owner actor that contain the state machine
	 *
	 * @return AActor*
	 */
	AActor* GetOwnerWorldObject();

	/**
	 * Execute internal function (Custom event mainly)
	 *
	 * @params FName FunctionName
	 * @params void* Params
	 */
	void ExecuteInnerFunction(FName FunctionName, void* Params = nullptr);

};
