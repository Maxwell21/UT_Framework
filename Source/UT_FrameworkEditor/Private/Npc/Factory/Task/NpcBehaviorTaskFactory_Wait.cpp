/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTaskFactory_Wait.h"
#include "NpcBehaviorTask_Wait.h"
#include "NpcBehaviorGraphNode_Wait.h"
#include "NpcBehaviorUtilities.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorTaskFactory"

FName FNpcBehaviorTaskFactory_Wait::GetName()
{
	return UNpcBehaviorTask_Wait::StaticClass()->GetFName();
}

FText FNpcBehaviorTaskFactory_Wait::GetNodeName()
{
	return LOCTEXT("NewWait", "New Wait");
}

FText FNpcBehaviorTaskFactory_Wait::GetNodeTooltip()
{
	return LOCTEXT("NewWaitTooltip", "Create a new Wait");
}

UEdGraphNode* FNpcBehaviorTaskFactory_Wait::Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin /*= nullptr*/)
{
	return Cast<UEdGraphNode>(FNpcBehaviorUtilities::AddNewTask<UNpcBehaviorGraphNode_Wait, UNpcBehaviorTask_Wait>(0, 0, Graph, FromPin, false));
}

#undef LOCTEXT_NAMESPACE
