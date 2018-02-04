/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "StateMachineBlueprint.generated.h"

class UStateMachine;

/**
 * 
 */
UCLASS(BlueprintType)
class UT_FRAMEWORK_API UStateMachineBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

#if WITH_EDITORONLY_DATA

	/** EdGraph based representation of the StateMachine */
	UPROPERTY()
	class UEdGraph* StateMachineGraph = nullptr;

	/** First node */
	UPROPERTY()
	class UEdGraphNode* EntryNode = nullptr;

	/** All nodes */
	UPROPERTY()
	TArray<class UEdGraphNode*> Nodes;

#endif	

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	UStateMachine* GetStateMachine();

#if WITH_EDITOR
	virtual UClass* GetBlueprintClass() const override;
#endif

};
