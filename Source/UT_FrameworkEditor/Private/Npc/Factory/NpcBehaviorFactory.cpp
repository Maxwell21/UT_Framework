/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorFactory.h"
#include "NpcBehaviorBlueprint.h"
#include "NpcBehaviorBlueprintGeneratedClass.h"
#include "NpcBehavior.h"
#include "Engine/Blueprint.h"
#include "KismetEditorUtilities.h"

UNpcBehaviorFactory::UNpcBehaviorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNpcBehaviorBlueprint::StaticClass();
	ParentClass = UNpcBehavior::StaticClass();
}

UObject* UNpcBehaviorFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	UNpcBehaviorBlueprint* NpcBehaviorBlueprint = CastChecked<UNpcBehaviorBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, Name, BPTYPE_Normal, UNpcBehaviorBlueprint::StaticClass(), UNpcBehaviorBlueprintGeneratedClass::StaticClass(), CallingContext));

	return NpcBehaviorBlueprint;
}

UObject* UNpcBehaviorFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(Class, InParent, Name, Flags, Context, Warn, NAME_None);
}

FName UNpcBehaviorFactory::GetNewAssetThumbnailOverride() const
{
	return TEXT("ClassThumbnail.NpcBehavior");
}
