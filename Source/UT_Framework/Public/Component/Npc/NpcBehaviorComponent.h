/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NpcBehaviorComponent.generated.h"

class UNpcBehavior;
class UGameplayTasksComponent;

UCLASS( ClassGroup=(Umbra), meta=(BlueprintSpawnableComponent) )
class UT_FRAMEWORK_API UNpcBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/**
	 * Template for create runtime instance
	 */
	UPROPERTY(Category = "Npc", EditDefaultsOnly)
	TSubclassOf<UNpcBehavior> NpcBehaviorTemplate;

	/**
	 * Runtime instance
	 */
	UPROPERTY()
	UNpcBehavior* NpcBehavior;

public:

	UPROPERTY()
	UGameplayTasksComponent* GameplayTaskComponent;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// Sets default values for this component's properties
	UNpcBehaviorComponent();

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	bool CreateNpcBehavior();

	bool CreateGameplayTaskComponent();

public:

	UFUNCTION(Category = "Npc | Component", BlueprintCallable)
	void StartNpcBehavior();

};
