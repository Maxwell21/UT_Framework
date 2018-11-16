/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineBlueprint.h"
#include "StateMachineBlueprintGeneratedClass.h"
#include "StateMachine.h"

UStateMachine* UStateMachineBlueprint::GetStateMachine()
{
	UStateMachine* Sm = Cast<UStateMachine>(this->GeneratedClass.GetDefaultObject());
	return Sm;
}

#if WITH_EDITOR
UClass* UStateMachineBlueprint::GetBlueprintClass() const
{
	return UStateMachineBlueprintGeneratedClass::StaticClass();
}

bool UStateMachineBlueprint::SupportsInputEvents() const
{
	return true;
}

#endif