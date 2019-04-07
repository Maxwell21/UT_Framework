/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineBlueprintCompiler.h"
#include "StateMachineBlueprintGeneratedClass.h"
#include "StateMachineBlueprint.h"
#include "KismetReinstanceUtilities.h"

FStateMachineBlueprintCompiler::FStateMachineBlueprintCompiler(UStateMachineBlueprint* SourceSketch, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions)
	: Super(SourceSketch, InMessageLog, InCompilerOptions)
{
}

FStateMachineBlueprintCompiler::~FStateMachineBlueprintCompiler()
{
}

void FStateMachineBlueprintCompiler::SpawnNewClass(const FString& NewClassName)
{
	StateMachineBlueprintGeneratedClass = FindObject<UStateMachineBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);

	if (StateMachineBlueprintGeneratedClass == nullptr)
		StateMachineBlueprintGeneratedClass = NewObject<UStateMachineBlueprintGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
	else
		FBlueprintCompileReinstancer::Create(StateMachineBlueprintGeneratedClass);

	NewClass = StateMachineBlueprintGeneratedClass;
}

void FStateMachineBlueprintCompiler::PostCompile()
{
	Super::PostCompile();
}
