/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "NpcBehaviorBlueprintGeneratedClass.h"
#include "Serialization/TextReferenceCollector.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ObjectResource.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/LinkerLoad.h"
#include "Engine/StreamableManager.h"

UNpcBehaviorBlueprintGeneratedClass::UNpcBehaviorBlueprintGeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNpcBehaviorBlueprintGeneratedClass::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
}

void UNpcBehaviorBlueprintGeneratedClass::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

UObject* UNpcBehaviorBlueprintGeneratedClass::CreateDefaultObject()
{
	return Super::CreateDefaultObject();
}

void UNpcBehaviorBlueprintGeneratedClass::PostLoad()
{
	Super::PostLoad();
}

bool UNpcBehaviorBlueprintGeneratedClass::NeedsLoadForServer() const
{
	return false;
}

void UNpcBehaviorBlueprintGeneratedClass::PurgeClass(bool bRecompilingOnLoad)
{
	Super::PurgeClass(bRecompilingOnLoad);
}
