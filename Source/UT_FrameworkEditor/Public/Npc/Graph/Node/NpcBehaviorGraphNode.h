/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "NpcBehaviorGraphNode.generated.h"

class UNpcBehaviorTask_Default;

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UNpcBehaviorGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

protected:

	bool IsRootNode;

	/* Default input and output pins */
	UEdGraphPin* InputPin;
	UEdGraphPin* OutputPin;

public:

	UPROPERTY()
	class UNpcBehaviorTask_Default* Task;

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
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin);
	virtual void ResetLinkedTask(UEdGraphPin* Pin = nullptr);
	virtual void DestroyNode() override;
	virtual void SaveNode();
	virtual void OnNotifyGraphChanged(); 
	virtual void OnPropertyUpdated(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged);
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface.

	virtual void ReallocateDefaultPins();
	
};
