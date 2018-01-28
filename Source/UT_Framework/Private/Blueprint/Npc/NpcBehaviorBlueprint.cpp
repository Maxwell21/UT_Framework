/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorBlueprint.h"
#include "NpcBehavior.h"

UNpcBehavior* UNpcBehaviorBlueprint::GetNpcBehavior()
{
	return this->GeneratedClass->GetDefaultObject<UNpcBehavior>();
}
