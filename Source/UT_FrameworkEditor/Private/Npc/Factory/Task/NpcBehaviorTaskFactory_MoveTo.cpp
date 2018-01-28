/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTaskFactory_MoveTo.h"
#include "NpcBehaviorTask_MoveTo.h"
#include "NpcBehaviorGraphNode_MoveTo.h"
#include "NpcBehaviorUtilities.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorTaskFactory"

FName FNpcBehaviorTaskFactory_MoveTo::GetName()
{
	return UNpcBehaviorTask_MoveTo::StaticClass()->GetFName();
}

FText FNpcBehaviorTaskFactory_MoveTo::GetNodeName()
{
	return LOCTEXT("NewMoveTo", "New Move To");
}

FText FNpcBehaviorTaskFactory_MoveTo::GetNodeTooltip()
{
	return LOCTEXT("NewMoveToTooltip", "Create a new Move To");
}

UEdGraphNode* FNpcBehaviorTaskFactory_MoveTo::Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin /*= nullptr*/)
{
	return Cast<UEdGraphNode>(FNpcBehaviorUtilities::AddNewTask<UNpcBehaviorGraphNode_MoveTo, UNpcBehaviorTask_MoveTo>(0, 0, Graph, FromPin, false));
}

#undef LOCTEXT_NAMESPACE
