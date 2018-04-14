/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "StateMachineBlueprintGeneratedClass.h"
#include "StateMachine.generated.h"

class UStateMachineState_Default;
class UStateMachineState_Entry;
class UStateMachineBlueprintGeneratedClass;
class UWorld;

USTRUCT(BlueprintType, Blueprintable)
struct FTransition
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/**
	* Main name of the transition (using also in the graph editor as node title)
	* Example : "Transition_State1ToState2"
	*/
	UPROPERTY()
	FString Name;

	/** From state name */
	UPROPERTY()
	FString FromState;

	/** To state name */
	UPROPERTY()
	FString ToState;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FTransition();

	bool IsValid();
	
};

USTRUCT(BlueprintType, Blueprintable)
struct FState
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/** Main name of the state (using also in the graph editor as node title) */
	UPROPERTY()
	FString Name;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString BeginFunctionName;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString UpdateFunctionName;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString FinishFunctionName;

	/** All transitions to others states */
	UPROPERTY()
	TArray<FString> TransitionNames;

	/** Using for determine which state is the entry point) */
	UPROPERTY()
	bool Root;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FState();

	bool IsValid();
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UT_FRAMEWORK_API UStateMachine : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/** All states */
	UPROPERTY()
	TArray<FState> States;

	/** All transitions */
	UPROPERTY()
	TMap<FString, FTransition> Transitions;

	/** Current state actually running */
	UPROPERTY()
	FState CurrentState;

	/** Current owner */
	UPROPERTY(Category = "State Machine", BlueprintReadOnly)
	UObject* OwnerObject;

	/**
	* If true the "Update" function of current state will be triggered 
	*
	* CAUTION : if this property is false transitions will not work
	*/
	UPROPERTY(Category = "State Machine", EditDefaultsOnly)
	bool UseTick = true;

	/** Delay between each check for all transitions for the current state */
	UPROPERTY(Category = "State Machine", EditDefaultsOnly)
	float TransitionCheckRate = 0.5f;

	/** Delta time ticked */
	UPROPERTY()
	float Delta;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Construct a new state machine from a template
	*
	* @params TSubclassOf<UStateMachineBlueprint> Template
	* @params UObject* Owner
	*
	* @return UStateMachine*
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	static UStateMachine* ConstructStateMachine(TSubclassOf<UStateMachine> Template, UObject* Owner);

protected:

	/**
	 * Initialize state machine
	 *
	 * @params UObject* Owner
	 */
	virtual void Init(UObject* Owner);

	/**
	 * Timer callback checking all transitions for current running state
	 */
	UFUNCTION()
	void CheckTransitionForCurrentState();

public:

	/**
	* Start state machine
	*/
	UFUNCTION(Category="Umbra Framework | State Machine", BlueprintCallable)
	virtual void Start();

	/**
	 * Get the current world that contain the state machine
	 *
	 * @return UWorld*
	 */
	UWorld* GetOwnerWorldObject();

	/**
	 * Execute internal function (Custom event mainly)
	 *
	 * @params FName FunctionName
	 * @params void* Params
	 */
	void ExecuteInnerFunction(FName FunctionName, void* Params = nullptr);

	/**
	* @return UStateMachineBlueprintGeneratedClass*
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	UStateMachineBlueprintGeneratedClass* GetGeneratedClass();

	/**
	 * Get State by a unique name
	 *
	 * @params FString Name
	 *
	 * @return FState
	 */
	FState GetStateByName(FString Name);

	/**
	* @params FState State
	*/
	void SetCurrentState(FState State);

	/**
	* Determine the root state
	*
	* @params FString Name
	*/
	void SetRootState(FString Name);

	/**
	* Get existing or empty root state
	*
	* @return FState
	*/
	FState GetRootState();

	/**
	 * Check if state exist, if so that return the id otherwise -1
	 */
	int32 StateExist(FString Name);

	/**
	* @params FState State
	* @params FString OldName
	 * 
	 * @return bool
	 */
	bool AddOrUpdateState(FState State, FString OldName = "");

	/**
	* @params FString Name
	*
	* @return bool
	*/
	bool RemoveStateByName(FString Name);

	/**
	* Get Transition by a unique name
	*
	* @params FString Name
	*
	* @return FTransition
	*/
	FTransition GetTransitionByName(FString Name);

	/**
	* @params FTransition Transition
	* @params FString OldName
	*
	* @return bool
	*/
	bool AddOrUpdateTransition(FTransition Transition, FString OldName = "");

	/**
	* @params FString Name
	*
	* @return bool
	*/
	bool RemoveTransitionByName(FString Name);

	/** FTickableGameObject */
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	/** End FTickableGameObject */

protected:

	/**
	* Internally call
	*/
	UFUNCTION()
	virtual void BeginState();

	/**
	* Internally call
	*/
	UFUNCTION()
	virtual void UpdateState(float DeltaTime);

	/**
	* Internally call
	*/
	UFUNCTION()
	virtual void FinishState();

	/************************************************************************/
	/* EVENTS                                                               */
	/************************************************************************/

public:

	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintImplementableEvent)
	void OnStateMachineStart();
	
};
