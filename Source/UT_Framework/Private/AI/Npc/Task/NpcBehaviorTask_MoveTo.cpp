/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_MoveTo.h"
#include "NpcBehavior.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AIController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UNpcBehaviorTask_MoveTo::Execute()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Execute move to task"));
#endif

	if (this->IsValid())
	{
		this->NavSystem = UNavigationSystem::GetCurrent(this->OwnerNpcBehavior->GetOwnerWorldObject());
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(this->OwnerNpcBehavior->GetOwnerWorldObject());
		if (this->NavSystem && World)
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, "TickTask", World->GetDeltaSeconds());
			World->GetTimerManager().SetTimer(TimerHandle, TimerDel, World->GetDeltaSeconds(), true);
		}
	}
}

void UNpcBehaviorTask_MoveTo::TickTask(float DeltaTime)
{
	if (this->NavSystem && !this->GoalReached)
	{
		this->Target = (this->GoalTarget) ? this->GoalTarget->GetActorLocation() : this->GoalLocation;
		this->NavSystem->SimpleMoveToLocation(this->OwnerNpcBehavior->OwnerController, Target);
		
		FVector MyVector(this->OwnerNpcBehavior->GetOwnerWorldObject()->GetActorLocation() - this->Target);
		
		if (MyVector.Size() < 25)
		{
			this->GoalReached = true;
			this->ExecuteNextTask();
		}
	}
}
