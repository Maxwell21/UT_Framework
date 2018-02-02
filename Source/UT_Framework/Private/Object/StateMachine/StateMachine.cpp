/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachine.h"
#include "StateMachineBlueprint.h"
#include "StateMachineState_Entry.h"
#include "StateMachineState_Default.h"
#include "GameFramework/Actor.h"

UStateMachine* UStateMachine::ConstructStateMachine(UStateMachine* Template, UObject* Owner)
{
	if (!Template || !Owner)
		return nullptr;

	if (UStateMachine* StateMachine = Cast<UStateMachine>(DuplicateObject(Template, Owner)))
	{
		StateMachine->Init(Owner);

		return StateMachine;
	}

	return nullptr;
}

UStateMachine* UStateMachine::CreateStateMachine(UStateMachineBlueprint* Template, UObject* Owner)
{
	if (!Template || !Owner)
		return nullptr;

	if (UStateMachine* StateMachine = Cast<UStateMachine>(Template->GeneratedClass.GetDefaultObject()))
	{
		StateMachine->Init(Owner);

		return StateMachine;
	}

	return nullptr;
}

void UStateMachine::Init(UObject* Owner)
{
	if (!Owner)
		return;

	this->OwnerObject = Owner;
}

void UStateMachine::Start()
{
	for (TFieldIterator<UFunction> Itr(GetClass()); Itr; ++Itr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetName());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Itr->GetFullName());
		UE_LOG(LogTemp, Warning, TEXT("--------"));
	}

	if (this->EntryState && this->EntryState->State)
	{
		this->EntryState->State->Init(this);
		this->EntryState->State->BeginState();
	}
}

void UStateMachine::ExecuteInnerFunction(FName FunctionName, void* Params/* = nullptr*/)
{
	if (UFunction* Function = this->FindFunction(FunctionName))
		this->ProcessEvent(Function, Params);
}

AActor* UStateMachine::GetOwnerWorldObject()
{
	return nullptr;
}
