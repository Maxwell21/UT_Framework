/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameworkGenerics.h"
#include "StateMachine.h"
#include "StateMachineState_Default.generated.h"

class UStateMachineTransition_Default;

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UStateMachineState_Default : public UObject
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	/** Main name of the state (using also in the graph editor as node title) */
	UPROPERTY()
	FState RuntimeData;

	/** All transitions to others states */
	UPROPERTY()
	TArray<UStateMachineTransition_Default*> Transitions;

#if WITH_EDITORONLY_DATA

	/** Current GraphNode owner */
	class UEdGraphNode* GraphNode = nullptr;

	/** EdGraph based representation of the State */
	UPROPERTY()
	class UEdGraph* StateGraph = nullptr;

	/** Node data (position) */
	UPROPERTY()
	FGenericNodeData NodeData;

	/* Current blueprint edited on the custom editor */
	class UStateMachineBlueprint* EditedStateMachineBlueprint;

#endif

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

};
