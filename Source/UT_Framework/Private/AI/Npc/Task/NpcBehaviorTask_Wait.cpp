/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_Wait.h"
#include "NpcBehavior.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UNpcBehaviorTask_Wait::Execute()
{
	#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("Execute wait task << %f >>"), this->Time);
	#endif

	if (this->IsValid())
	{
		if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this->OwnerNpcBehavior->GetOwnerWorldObject()))
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerCallback;
			TimerCallback.BindLambda([](UNpcBehaviorTask_Wait* WaitTask)
			{
				WaitTask->ExecuteNextTask();
			}, this);

			World->GetTimerManager().SetTimer(TimerHandle, TimerCallback, this->Time, false);
		}
	}
}
