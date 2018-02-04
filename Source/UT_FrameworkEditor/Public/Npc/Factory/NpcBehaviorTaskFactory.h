/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"

class UEdGraphNode;
class UNpcBehaviorGraph;
class UEdGraphPin;

class UT_FRAMEWORKEDITOR_API FNpcBehaviorTaskFactory
{
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

	virtual	FName GetName();

	virtual	FText GetNodeName();

	virtual	FText GetNodeTooltip();

	virtual UEdGraphNode* Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin = nullptr);

};

