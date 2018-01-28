/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTaskFactory_Restart.h"
#include "NpcBehaviorTask_Restart.h"
#include "NpcBehaviorGraphNode_Restart.h"
#include "NpcBehaviorUtilities.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorTaskFactory"

FName FNpcBehaviorTaskFactory_Restart::GetName()
{
	return UNpcBehaviorTask_Restart::StaticClass()->GetFName();
}

FText FNpcBehaviorTaskFactory_Restart::GetNodeName()
{
	return LOCTEXT("NewRestart", "New Restart");
}

FText FNpcBehaviorTaskFactory_Restart::GetNodeTooltip()
{
	return LOCTEXT("NewRestartTooltip", "Create a new Restart");
}

UEdGraphNode* FNpcBehaviorTaskFactory_Restart::Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin /*= nullptr*/)
{
	return Cast<UEdGraphNode>(FNpcBehaviorUtilities::AddNewTask<UNpcBehaviorGraphNode_Restart, UNpcBehaviorTask_Restart>(0, 0, Graph, FromPin, false));
}

#undef LOCTEXT_NAMESPACE
