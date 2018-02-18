/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorBlueprint.h"
#include "NpcBehaviorBlueprintGeneratedClass.h"
#include "NpcBehavior.h"

UNpcBehavior* UNpcBehaviorBlueprint::GetNpcBehavior()
{
	UNpcBehavior* NpcBehavior = this->GeneratedClass->GetDefaultObject<UNpcBehavior>();
	return NpcBehavior;
}

#if WITH_EDITOR
UClass* UNpcBehaviorBlueprint::GetBlueprintClass() const
{
	return UNpcBehaviorBlueprintGeneratedClass::StaticClass();
}
#endif
