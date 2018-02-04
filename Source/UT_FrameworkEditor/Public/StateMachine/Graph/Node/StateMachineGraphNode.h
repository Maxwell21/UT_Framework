/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "StateMachineGraphNode.generated.h"

class UStateMachineState_Default;

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UStateMachineGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

protected:

	bool IsRootNode;

	/* Default input and output pins */
	UEdGraphPin* InputPin;
	UEdGraphPin* OutputPin;

public:

	UPROPERTY()
	UStateMachineState_Default* State;

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	virtual void SetIsRootNode(bool IsNewRootNode);
	virtual bool GetRootNode() { return this->IsRootNode; };

	/** Get the Pin that connects to parent */
	virtual UEdGraphPin* GetInputPin() const { return InputPin; }
	/** Get the Pin that connects to children */
	virtual UEdGraphPin* GetOutputPin() const { return OutputPin; }

	// UEdGraphNode interface.
	virtual void ReconstructNode() override;
	virtual	void AllocateDefaultPins() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin);
	virtual void DestroyNode() override;
	virtual void SaveNode();
	virtual void OnNotifyGraphChanged(); 
	virtual void OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged);
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface.

	virtual FString GetStateName() const;
	virtual void ReallocateDefaultPins();
	virtual void GetTransitionList(TArray<class UStateMachineGraphNode_Transition*>& OutTransitions, bool bWantSortedList = false);
	virtual void CreateBoundGraph();

};
