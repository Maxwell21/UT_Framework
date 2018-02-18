/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorBlueprintCompiler.h"
#include "NpcBehaviorBlueprintGeneratedClass.h"
#include "NpcBehaviorBlueprint.h"
#include "KismetReinstanceUtilities.h"

FNpcBehaviorBlueprintCompiler::FNpcBehaviorBlueprintCompiler(UNpcBehaviorBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions, TArray<UObject*>* InObjLoaded)
	: Super(SourceSketch, InMessageLog, InCompilerOptions, InObjLoaded)
{
}

FNpcBehaviorBlueprintCompiler::~FNpcBehaviorBlueprintCompiler()
{
}

void FNpcBehaviorBlueprintCompiler::SpawnNewClass(const FString& NewClassName)
{
	NpcBehaviorBlueprintGeneratedClass = FindObject<UNpcBehaviorBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);

	if (NpcBehaviorBlueprintGeneratedClass == nullptr)
		NpcBehaviorBlueprintGeneratedClass = NewObject<UNpcBehaviorBlueprintGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
	else
		FBlueprintCompileReinstancer::Create(NpcBehaviorBlueprintGeneratedClass);

	NewClass = NpcBehaviorBlueprintGeneratedClass;
}

void FNpcBehaviorBlueprintCompiler::CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO)
{
	UNpcBehaviorBlueprintGeneratedClass* NpcBehaviorBlueprintGeneratedClass = Cast<UNpcBehaviorBlueprintGeneratedClass>(ClassToClean);
	UNpcBehavior* NpcBehavior = Cast<UNpcBehavior>(NpcBehaviorBlueprintGeneratedClass->GetDefaultObject());

	Super::CleanAndSanitizeClass(ClassToClean, InOutOldCDO);
}

void FNpcBehaviorBlueprintCompiler::PostCompile()
{
	Super::PostCompile();
}
