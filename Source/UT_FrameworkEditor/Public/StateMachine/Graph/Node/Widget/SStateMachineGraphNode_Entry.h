/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "SStateMachineGraphNode.h"

class UStateMachineGraphNode;

/************************************************************************/
/* SStateMachineGraphNodeEntry_OutputPin		                        */
/************************************************************************/

class SStateMachineGraphNodeEntry_OutputPin : public SGraphPin
{

public:

	SLATE_BEGIN_ARGS(SStateMachineGraphNodeEntry_OutputPin) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:

	// Begin SGraphPin interface
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	const FSlateBrush* GetPinBorder() const;
	const FSlateBrush* GetPinImage() const;
	// End SGraphPin interface

};

/************************************************************************/
/* SStateMachineGraphNode_Entry											*/
/************************************************************************/

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SStateMachineGraphNode_Entry : public SStateMachineGraphNode
{

public:

	SLATE_BEGIN_ARGS(SStateMachineGraphNode_Entry) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UStateMachineGraphNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
 	// End of SGraphNode interface

};
