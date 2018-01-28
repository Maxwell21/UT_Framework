/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorUtilities.h"
#include "NpcBehaviorTaskFactory.h"
#include "NpcBehaviorTask_Default.h"
#include "UObjectIterator.h"

TArray<TSharedPtr<FNpcBehaviorTaskFactory>> FNpcBehaviorUtilities::RegisteredTasks;

void FNpcBehaviorUtilities::RegisterTask(TSharedPtr<FNpcBehaviorTaskFactory> NodeFactory)
{
	if (NodeFactory.IsValid() && NodeFactory->GetName().IsValid())
		RegisteredTasks.Add(NodeFactory);
}

TSharedPtr<FNpcBehaviorTaskFactory> FNpcBehaviorUtilities::GetRegisteredTask(FName TaskName)
{
	for (auto const& Task : RegisteredTasks)
	{
		if (Task.IsValid() && Task->GetName() == TaskName)
			return Task;
	}

	return nullptr;
}

TArray<TSharedPtr<FNpcBehaviorTaskFactory>> FNpcBehaviorUtilities::GetAllRegisteredTasks()
{
	return RegisteredTasks;
}
