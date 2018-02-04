/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NpcBehaviorTask_Default.h"
#include "NpcBehaviorTask_MoveTo.generated.h"

class UNavigationSystem;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_MoveTo : public UNpcBehaviorTask_Default
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY(Category = "Npc MoveTo", EditDefaultsOnly)
	FVector GoalLocation;

	UPROPERTY(Category = "Npc MoveTo", EditDefaultsOnly)
	AActor* GoalTarget;

	FVector Target;

	UNavigationSystem* NavSystem;

	bool GoalReached = false;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void Execute() override;

	UFUNCTION()
	virtual void TickTask(float DeltaTime) override;

};
