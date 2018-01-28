/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_Entry.h"
#include "Engine/Engine.h"

void UNpcBehaviorTask_Entry::Execute()
{
	#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("START NPC BEHAVIOR"));
	#endif

	// automatically execute the next task 
	this->ExecuteNextTask();
}
