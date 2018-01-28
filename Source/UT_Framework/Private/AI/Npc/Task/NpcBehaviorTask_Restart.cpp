/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_Restart.h"
#include "NpcBehavior.h"

void UNpcBehaviorTask_Restart::Execute()
{
	#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("Execute restart task"));
	#endif

	if (this->IsValid() && this->OwnerNpcBehavior->EntryTask)
	{
		this->NextTask = Cast<UNpcBehaviorTask_Default>(this->OwnerNpcBehavior->EntryTask);
		this->ExecuteNextTask();
	}
}
