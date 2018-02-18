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
#include "App.h"

void UNpcBehaviorTask_MoveTo::Execute()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Execute move to task"));
#endif

	if (this->IsValid())
	{
		this->GoalReached = false;
		this->NavSystem = UNavigationSystem::GetCurrent(this->OwnerNpcBehavior->GetOwnerWorldObject());
		UWorld* World = GEngine->GetWorldFromContextObjectChecked(this->OwnerNpcBehavior->GetOwnerWorldObject());
		if (this->NavSystem && World)
		{
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDel;
			float Rate = FApp::GetDeltaTime();
			TimerDel.BindUFunction(this, "TickTask", Rate);
			World->GetTimerManager().SetTimer(TimerHandle, TimerDel, Rate, true);
		}
	}
}

void UNpcBehaviorTask_MoveTo::TickTask(float DeltaTime)
{
	if (this->NavSystem && !this->GoalReached)
	{
		this->SetTargetLocation();
		this->NavSystem->SimpleMoveToLocation(this->OwnerNpcBehavior->OwnerController, Target);
		FVector MyVector(this->OwnerNpcBehavior->GetOwnerWorldObject()->GetActorLocation() - this->Target);
		
		if (MyVector.Size() < 45)
		{
			this->GoalReached = true;
			this->ExecuteNextTask();
		}
	}
}

void UNpcBehaviorTask_MoveTo::SetTargetLocation()
{
	for (TFieldIterator<UProperty> PropIt(this->OwnerNpcBehavior->GetClass(), EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
	{
		UProperty* Property = *PropIt;
		if (Property->GetName() == this->TargetVariable.ToString())
		{
			if (UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property))
			{
				if (AActor* Actor = Cast<AActor>(ObjectProperty->GetPropertyValue_InContainer(this->OwnerNpcBehavior)))
					this->Target = Actor->GetActorLocation();
			}
			else
			{
				if (FVector* Location = Property->ContainerPtrToValuePtr<FVector>(this->OwnerNpcBehavior))
					this->Target = *Location;
			}
		}
	}
}
