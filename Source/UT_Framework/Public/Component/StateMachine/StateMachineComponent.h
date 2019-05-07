/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine.h"
#include "StateMachineComponent.generated.h"

class UStateMachine;

UCLASS(ClassGroup=(UmbraTools), meta=(BlueprintSpawnableComponent))
class UT_FRAMEWORK_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	/************************************************************************/
	/* DELEGATES                                                            */
	/************************************************************************/

	UPROPERTY(Category = "State Machine", BlueprintAssignable)
	FStateMachineDelegate OnInit;

	UPROPERTY(Category = "State Machine", BlueprintAssignable)
	FStateMachineDelegate OnPause;

	UPROPERTY(Category = "State Machine", BlueprintAssignable)
	FStateMachineDelegate OnUnPause;

	UPROPERTY(Category = "State Machine", BlueprintAssignable)
	FStateMachineDelegate OnStart;

	UPROPERTY(Category = "State Machine", BlueprintAssignable)
	FStateMachineDelegate OnStop;

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "State Machine", EditAnywhere)
	TSubclassOf<UStateMachine> Template;

	UPROPERTY()
	UStateMachine* StateMachine;

	UPROPERTY(Category = "State Machine", EditAnywhere)
	bool AutoStart;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// Sets default values for this component's properties
	UStateMachineComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:

	/**
	* Start state machine
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	virtual void Start();

	/**
	* Stop state machine
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	virtual void Stop();

	/**
	* @return FState
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable, BlueprintPure)
	FState GetCurrentState();

	/**
	* @return FString
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable, BlueprintPure)
	FString GetCurrentStateName();

	/**
	* @return UStateMachine*
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable, BlueprintPure)
	UStateMachine* GetMachine();

	/**
	* Paused state machine
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	virtual void Pause();

	/**
	* UnPaused state machine
	*/
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintCallable)
	virtual void UnPaused();

	/**
	 * @return bool - If paused or not
	 */
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintPure)
	virtual bool IsPaused() const;

	/**
	 * @return bool - If is valid or not
	 */
	UFUNCTION(Category = "Umbra Framework | State Machine", BlueprintPure)
	virtual bool IsValid() const;

protected:

	virtual void BindDelegates();

	virtual void UnbindDelegates();

	/************************************************************************/
	/* DELEGATES BOUNDS                                                     */
	/************************************************************************/

	UFUNCTION()
	void OnInitTriggered();

	UFUNCTION()
	void OnStartTriggered();

	UFUNCTION()
	void OnStopTriggered();

	UFUNCTION()
	void OnPauseTriggered();

	UFUNCTION()
	void OnUnPauseTriggered();

};
