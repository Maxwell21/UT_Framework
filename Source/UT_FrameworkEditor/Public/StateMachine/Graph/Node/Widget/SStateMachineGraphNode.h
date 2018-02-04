/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SNodePanel.h"
#include "SGraphNode.h"
#include "SGraphPin.h"

class UStateMachineGraphNode;

/************************************************************************/
/* SStateMachineGraphNode_OutputPin		                                */
/************************************************************************/

class SStateMachineGraphNode_OutputPin : public SGraphPin
{

public:

	SLATE_BEGIN_ARGS(SStateMachineGraphNode_OutputPin) {}
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
/* SStateMachineGraphNode												*/
/************************************************************************/

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SStateMachineGraphNode : public SGraphNode
{

public:
	
	SLATE_BEGIN_ARGS(SStateMachineGraphNode) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UStateMachineGraphNode* InNode);
	
	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	// End of SGraphNode interface

	virtual FSlateColor GetNodeColor() const;
	virtual FSlateColor GetNodeBackgroundColor() const;
	virtual FText GetNodeTitle() const;
	FText GetPreviewCornerText() const;
	const FSlateBrush* GetNameIcon() const;
	FSlateColor GetBorderBackgroundColor() const;
};
