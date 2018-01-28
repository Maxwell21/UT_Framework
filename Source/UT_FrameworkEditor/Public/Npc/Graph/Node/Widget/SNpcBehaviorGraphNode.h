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

class UNpcBehaviorGraphNode;

/************************************************************************/
/* SNpcBehaviorGraphNode_OutputPin		                                */
/************************************************************************/

class SNpcBehaviorGraphNode_OutputPin : public SGraphPin
{

public:

	SLATE_BEGIN_ARGS(SNpcBehaviorGraphNode_OutputPin) {}
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
/* SNpcBehaviorGraphNode												*/
/************************************************************************/

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SNpcBehaviorGraphNode : public SGraphNode
{

public:

	SLATE_BEGIN_ARGS(SNpcBehaviorGraphNode) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UNpcBehaviorGraphNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	virtual const FSlateBrush* GetShadowBrush(bool bSelected) const override;
	// End of SGraphNode interface

	virtual TSharedRef<class SWidget> GetNodeContent();
	virtual FSlateColor GetNodeColor() const;
	virtual FSlateColor GetNodeBackgroundColor() const;
	virtual FText GetNodeTitle() const;

};
