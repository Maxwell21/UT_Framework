/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "NpcBehaviorTaskFactory.h"

class UEdGraphNode;
class UNpcBehaviorGraph;
class UEdGraphPin;

class UT_FRAMEWORKEDITOR_API FNpcBehaviorTaskFactory_Wait : public FNpcBehaviorTaskFactory
{
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

	virtual	FName GetName() override;

	virtual FText GetNodeName() override;

	virtual FText GetNodeTooltip() override;

	virtual UEdGraphNode* Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin = nullptr) override;

};

