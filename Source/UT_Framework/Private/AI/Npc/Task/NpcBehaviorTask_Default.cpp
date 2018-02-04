/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_Default.h"
#include "NpcBehavior.h"
#include "GameplayTaskOwnerInterface.h"

void UNpcBehaviorTask_Default::Initialize(UNpcBehavior* InOwnerNpcBehavior)
{
	this->OwnerNpcBehavior = InOwnerNpcBehavior;

	if (this->OwnerNpcBehavior)
	{
		// Init gameplay task
		this->InitTask(*GetNpcBehaviorTaskOwner(), GetNpcBehaviorTaskOwner()->GetGameplayTaskDefaultPriority());

		// Execute task
		this->Execute();
	}
}

void UNpcBehaviorTask_Default::ExecuteNextTask()
{
	if (this->IsValid() && this->NextTask)
	{
		//this->EndTask();
		this->NextTask->Initialize(OwnerNpcBehavior);
	}
}

void UNpcBehaviorTask_Default::Execute()
{
	// Default Task do nothing
}

bool UNpcBehaviorTask_Default::IsValid()
{
	return this->OwnerNpcBehavior;
}

IGameplayTaskOwnerInterface* UNpcBehaviorTask_Default::GetNpcBehaviorTaskOwner()
{
	return Cast<IGameplayTaskOwnerInterface>(this->OwnerNpcBehavior);
}
