/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "NpcBehaviorBlueprint.generated.h"

class UNpcBehavior;

/**
 * 
 */
UCLASS(BlueprintType)
class UT_FRAMEWORK_API UNpcBehaviorBlueprint : public UBlueprint
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

#if WITH_EDITORONLY_DATA

	/** EdGraph based representation of the NpcBehavior */
	UPROPERTY()
	class UEdGraph* NpcBehaviorGraph = nullptr;

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

	UNpcBehavior* GetNpcBehavior();
};
