/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "NpcBehaviorBlueprint.h"
#include "KismetCompiler.h"

class UNpcBehaviorBlueprint;
class UNpcBehaviorBlueprintGeneratedClass;

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API FNpcBehaviorBlueprintCompiler : public FKismetCompilerContext
{

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

protected:

	typedef FKismetCompilerContext Super;

	UNpcBehaviorBlueprintGeneratedClass* NpcBehaviorBlueprintGeneratedClass;

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	FNpcBehaviorBlueprintCompiler (UNpcBehaviorBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions, TArray<UObject*>* InObjLoaded);
	virtual ~FNpcBehaviorBlueprintCompiler();

	// FKismetCompilerContext
// 	virtual UEdGraphSchema_K2* CreateSchema() override;
// 	virtual void CreateFunctionList() override;
	virtual void SpawnNewClass(const FString& NewClassName) override;
// 	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
// 	virtual void PrecompileFunction(FKismetFunctionContext& Context, EInternalCompilerFlags InternalFlags) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOutOldCDO) override;
// 	virtual void SaveSubObjectsFromCleanAndSanitizeClass(FSubobjectCollection& SubObjectsToSave, UBlueprintGeneratedClass* ClassToClean) override;
// 	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
// 	virtual void CreateClassVariablesFromBlueprint() override;
// 	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject);
// 	virtual void FinishCompilingClass(UClass* Class) override;
// 	virtual bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;
	virtual void PostCompile() override;
	// End FKismetCompilerContext
};
