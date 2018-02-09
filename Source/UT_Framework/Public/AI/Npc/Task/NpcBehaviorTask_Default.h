/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameworkGenerics.h"
#include "GameplayTask.h"
#include "NpcBehaviorTask_Default.generated.h"

class UNpcBehavior;
class IGameplayTaskOwnerInterface;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UNpcBehaviorTask_Default : public UGameplayTask
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UPROPERTY()
	UNpcBehaviorTask_Default* NextTask;

	UPROPERTY()
	UNpcBehavior* OwnerNpcBehavior;

#if WITH_EDITORONLY_DATA

	/** Current GraphNode owner */
	class UEdGraphNode* GraphNode = nullptr;

	/** Node data (position) */
	UPROPERTY()
	FGenericNodeData NodeData;

	/* Current blueprint edited on the custom editor */
	class UNpcBehaviorBlueprint* EditedNpcBlueprint;

#endif

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	 * Initialize and execute the task
	 *
	 * @param UNpcBehavior* InOwnerNpcBehavior
	 */
	virtual void Initialize(UNpcBehavior* InOwnerNpcBehavior);

	/**
	 * Call the next task
	 */
	virtual void ExecuteNextTask();

	/**
	 * Execute the current task
	 */
	virtual void Execute();

	/**
	 * Check if the task is valid before execution
	 */
	virtual bool IsValid();

	/**
	 * Shortcut for getting the NpcBehavior interface
	 */
	IGameplayTaskOwnerInterface* GetNpcBehaviorTaskOwner();
};
