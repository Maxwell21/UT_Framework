/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorTaskFactory_RandomBranch.h"
#include "NpcBehaviorTask_RandomBranch.h"
#include "NpcBehaviorGraphNode_RandomBranch.h"
#include "NpcBehaviorUtilities.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorTaskFactory"

FName FNpcBehaviorTaskFactory_RandomBranch::GetName()
{
	return UNpcBehaviorTask_RandomBranch::StaticClass()->GetFName();
}

FText FNpcBehaviorTaskFactory_RandomBranch::GetNodeName()
{
	return LOCTEXT("NewRandomBranch", "New RandomBranch");
}

FText FNpcBehaviorTaskFactory_RandomBranch::GetNodeTooltip()
{
	return LOCTEXT("NewRandomBranchTooltip", "Create a new RandomBranch");
}

UEdGraphNode* FNpcBehaviorTaskFactory_RandomBranch::Execute(UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin /*= nullptr*/)
{
	return Cast<UEdGraphNode>(FNpcBehaviorUtilities::AddNewTask<UNpcBehaviorGraphNode_RandomBranch, UNpcBehaviorTask_RandomBranch>(0, 0, Graph, FromPin, false));
}

#undef LOCTEXT_NAMESPACE
