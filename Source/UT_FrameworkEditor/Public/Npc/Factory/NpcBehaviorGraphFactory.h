/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraphUtilities.h"
#include "SGraphNode.h"

class UT_FRAMEWORKEDITOR_API FNpcBehaviorGraphFactory : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* InNode) const override;
};

