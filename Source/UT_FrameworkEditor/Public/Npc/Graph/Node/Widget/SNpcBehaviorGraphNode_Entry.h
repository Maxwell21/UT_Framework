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
class SNpcBehaviorGraphNode;

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SNpcBehaviorGraphNode_Entry : public SNpcBehaviorGraphNode
{

public:

	SLATE_BEGIN_ARGS(SNpcBehaviorGraphNode_Entry) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UNpcBehaviorGraphNode* InNode);

	// SGraphNode interface
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	// End of SGraphNode interface

};
