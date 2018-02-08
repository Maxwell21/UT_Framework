/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Transition.generated.h"

class UStateMachineState_Default;
class UStateMachineTransition_Default;
/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UStateMachineGraphNode_Transition : public UStateMachineGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	
	/************************************************************************/
	/* PROPERTIES															*/
	/************************************************************************/

	int32 PriorityOrder;

	UPROPERTY()
	UStateMachineTransition_Default* Transition;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	// UEdGraphNode interface.
	virtual void ReconstructNode() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual bool CanJumpToDefinition() const override;
	virtual void JumpToDefinition() const override;
	virtual void DestroyNode() override;
	// End UEdGraphNode interface.
	
	virtual void CreateBoundGraph() override;
	UStateMachineGraphNode* GetPreviousState() const;
	UStateMachineGraphNode* GetNextState() const;
	void CreateConnections(UStateMachineGraphNode* NodeA, UStateMachineGraphNode* NodeB);
	void LinkNode(UStateMachineGraphNode* NodeA, UStateMachineGraphNode* NodeB);
};
