/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "StateMachineBlueprintGeneratedClass.h"
#include "Serialization/TextReferenceCollector.h"
#include "UObject/EditorObjectVersion.h"
#include "UObject/ObjectResource.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"
#include "UObject/LinkerLoad.h"
#include "Engine/StreamableManager.h"

UStateMachineBlueprintGeneratedClass::UStateMachineBlueprintGeneratedClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UStateMachineBlueprintGeneratedClass::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);
}

void UStateMachineBlueprintGeneratedClass::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

UObject* UStateMachineBlueprintGeneratedClass::CreateDefaultObject()
{
	return Super::CreateDefaultObject();
}

void UStateMachineBlueprintGeneratedClass::PostLoad()
{
	Super::PostLoad();
}

bool UStateMachineBlueprintGeneratedClass::NeedsLoadForServer() const
{
	return false;
}

void UStateMachineBlueprintGeneratedClass::PurgeClass(bool bRecompilingOnLoad)
{
	Super::PurgeClass(bRecompilingOnLoad);
}
