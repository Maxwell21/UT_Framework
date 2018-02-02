/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FrameworkGenerics.h"
#include "StateMachineState_Default.generated.h"

class UStateMachine;
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
	FString StateName;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString BeginStateFunctionName;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString UpdateStateFunctionName;

	/** Function name related to the "Custom Event" inside the state graph */
	UPROPERTY()
	FString FinishStateFunctionName;

	/** All transitions to others states */
	UPROPERTY()
	TArray<UStateMachineTransition_Default*> Transitions;

	/** Current state machine owner */
	UStateMachine* OwnerStateMachine;

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

	void Init(UStateMachine* Owner);

	UFUNCTION()
	void BeginState();

	UFUNCTION()
	void UpdateState(float DeltaTime);

	UFUNCTION()
	void FinishState();

};
