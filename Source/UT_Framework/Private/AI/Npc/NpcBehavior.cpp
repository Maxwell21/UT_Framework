/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehavior.h"
#include "NpcBehaviorComponent.h"
#include "NpcBehaviorTask_Entry.h"
#include "NpcBehaviorTask_Default.h"
#include "GameFramework/Actor.h"
#include "GameplayTasksComponent.h"

void UNpcBehavior::Start(UNpcBehaviorComponent* InOwnerComponent, AAIController* InOwnerController)
{
	this->OwnerComponent  = InOwnerComponent;
	this->OwnerController = InOwnerController;

	// Start entry task
	if (this->EntryTask)
		this->EntryTask->Initialize(this);
}

AActor* UNpcBehavior::GetOwnerWorldObject()
{
	return (this->OwnerComponent) ? this->OwnerComponent->GetOwner() : nullptr;
}

// --------------------------------------
//	IGameplayTaskOwnerInterface
// --------------------------------------	

UGameplayTasksComponent* UNpcBehavior::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	if(this->OwnerComponent)
		return this->OwnerComponent->GameplayTaskComponent;

	return nullptr;
}

AActor* UNpcBehavior::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return Cast<AActor>(this->GetOuter());
}

AActor* UNpcBehavior::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return Cast<AActor>(this->GetOuter());
}

void UNpcBehavior::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	// ---
}

void UNpcBehavior::OnGameplayTaskActivated(UGameplayTask& Task)
{
	ActiveTasks.Add(&Task);
}

void UNpcBehavior::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	ActiveTasks.Add(&Task);
}
