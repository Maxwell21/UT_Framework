/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "NpcBehavior.generated.h"

class UNpcBehaviorTask_Entry;
class UNpcBehaviorComponent;
class UGameplayTask;
class UGameplayTasksComponent;
class AAIController;

/**
 * 
 */
UCLASS(Abstract)
class UT_FRAMEWORK_API UNpcBehavior : public UObject, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/
	
	UPROPERTY()
	class UNpcBehaviorTask_Entry* EntryTask;

	class UNpcBehaviorTask_Default* CurrentTask = nullptr;
	
	UPROPERTY()
	UNpcBehaviorComponent* OwnerComponent;

	UPROPERTY()
	AAIController* OwnerController;

	/*
	* Current Active tasks related to this NpcBehavior
	*/
	UPROPERTY()
	TArray<UGameplayTask*> ActiveTasks;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* @param UNpcBehaviorComponent* InOwnerComponent
	* @param AAIController* InOwnerController
	*/
	virtual void Start(UNpcBehaviorComponent* InOwnerComponent, AAIController* InOwnerController);

	/**
	 * Get the current actor owner contain the NpcBehaviorComponent
	 *
	 * @return AActor*
	 */
	AActor* GetOwnerWorldObject();

	// --------------------------------------
	//	IGameplayTaskOwnerInterface
	// --------------------------------------
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;

};
