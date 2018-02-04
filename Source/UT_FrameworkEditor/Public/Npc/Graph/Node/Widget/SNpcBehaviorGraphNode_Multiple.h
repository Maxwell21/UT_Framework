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
#include "SNpcBehaviorGraphNode.h"

class UNpcBehaviorGraphNode;

/************************************************************************/
/* SNpcBehaviorGraphNode_Multiple										*/
/************************************************************************/

/**
 * 
 */
class UT_FRAMEWORKEDITOR_API SNpcBehaviorGraphNode_Multiple : public SNpcBehaviorGraphNode
{

public:

	SLATE_BEGIN_ARGS(SNpcBehaviorGraphNode_Multiple) {}
	SLATE_END_ARGS()

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	void Construct(const FArguments& InArgs, UNpcBehaviorGraphNode* InNode);

	// SGraphNode interface
	virtual void CreatePinWidgets() override;
	// End of SGraphNode interface

};
