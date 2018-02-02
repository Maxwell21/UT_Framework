/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineBlueprint.h"
#include "StateMachine.h"

UStateMachine* UStateMachineBlueprint::GetStateMachine()
{
	if (!this->StateMachine)
		this->StateMachine = Cast<UStateMachine>(this->GeneratedClass.GetDefaultObject());
	
	for (TFieldIterator<UFunction> Itr(GetClass()); Itr; ++Itr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetFullName());
		UE_LOG(LogTemp, Warning, TEXT("--------"));
	}

	return this->StateMachine;
}
