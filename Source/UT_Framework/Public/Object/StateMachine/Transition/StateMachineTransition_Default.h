/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameworkGenerics.h"
#include "StateMachineTransition_Default.generated.h"

/**
 * 
 */
UCLASS()
class UT_FRAMEWORK_API UStateMachineTransition_Default : public UObject
{
	GENERATED_BODY()

public:

	/** 
	* Main name of the transition (using also in the graph editor as node title) 
	* Example : "Transition_State1ToState2"
	*/
	UPROPERTY()
	FString TransitionName;

	UPROPERTY()
	UStateMachineState_Default* FromState;

	UPROPERTY()
	UStateMachineState_Default* ToState;
	
	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

#if WITH_EDITORONLY_DATA

	/** Current GraphNode owner */
	class UEdGraphNode* GraphNode = nullptr;

	/** EdGraph based representation of the Transition */
	UPROPERTY()
	class UEdGraph* TransitionGraph = nullptr;

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
