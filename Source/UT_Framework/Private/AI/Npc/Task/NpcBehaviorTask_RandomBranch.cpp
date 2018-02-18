/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTask_RandomBranch.h"
#include "NpcBehavior.h"
#include "Kismet/KismetMathLibrary.h"

void UNpcBehaviorTask_RandomBranch::Execute()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Execute random branch task"));
#endif

	if (this->IsValid())
	{
		int32 NextTarget = UKismetMathLibrary::RandomIntegerInRange(0, this->Number);
		if (this->MultipleTargets.IsValidIndex(NextTarget))
			this->NextTask = this->MultipleTargets[NextTarget].NextTask;

		this->ExecuteNextTask();
	}
}
