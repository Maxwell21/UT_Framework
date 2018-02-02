/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineState_Default.h"
#include "StateMachine.h"

void UStateMachineState_Default::Init(UStateMachine* Owner)
{
	this->OwnerStateMachine = Owner;
}

void UStateMachineState_Default::BeginState()
{
	for (TFieldIterator<UFunction> Itr(GetClass()); Itr; ++Itr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetFullName());
		UE_LOG(LogTemp, Warning, TEXT("--------"));
	}

	this->OwnerStateMachine->ExecuteInnerFunction(*this->BeginStateFunctionName);
}

void UStateMachineState_Default::UpdateState(float DeltaTime)
{

}

void UStateMachineState_Default::FinishState()
{

}
