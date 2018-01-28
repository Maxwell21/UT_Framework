/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTaskFactory.h"
#include "NpcBehaviorGraph.h"

FName FNpcBehaviorTaskFactory::GetName()
{
	return FName();
}

FText FNpcBehaviorTaskFactory::GetNodeName()
{
	return FText();
}

FText FNpcBehaviorTaskFactory::GetNodeTooltip()
{
	return FText();
}

UEdGraphNode* FNpcBehaviorTaskFactory::Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin /*= nullptr*/)
{
	return nullptr;
}
