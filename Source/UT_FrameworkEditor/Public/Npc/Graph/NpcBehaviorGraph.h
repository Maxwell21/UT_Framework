/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "NpcBehaviorGraph.generated.h"

class UNpcBehaviorBlueprint;
class UNpcBehaviorTask_Default;
class UNpcBehaviorGraphNode;

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UNpcBehaviorGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UNpcBehaviorBlueprint* NpcBehaviorBlueprint;

protected:

	UNpcBehaviorGraphNode* EntryNode;

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Init properties for the graph and rebuild the graph
	*/
	void Init(UNpcBehaviorBlueprint* InNpcBehaviorBlueprint);

	/**
	* Completely rebuild the graph from the root, removing all old nodes
	*/
	void RebuildGraph();

	/**
	* Construct all nodes after loading the graph
	*/
	void ReconstructNodes();

	/**
	 * Register a new node to the graph and the task
	 */
	void RegisterNode(UNpcBehaviorTask_Default* Task, UNpcBehaviorGraphNode* NewNode, UEdGraphPin* FromPin);

	/**
	* Use this graph to re-link all of the tasks it represents after a change in linkage
	*/
	void LinkTasks();

	/**
	* Create the "Entry" node for begin the Npc behavior (purely cosmetic)
	*/
	void InitDefaultNode();

	/**
	* Automatically set the "Next" task object from previous task node pin with the new object
	*/
	void SetNextTask(UEdGraphPin* FromPin, UNpcBehaviorGraphNode* ToNode);

	/**
	* Notify all nodes that graph has changed
	*/
	void NotifyNodesGraphChanged();

	/**
	* Get the right outer for creating the new Task
	*/
	UObject* PrepareOuter();

protected:

	/**
	* Check if the entry node is set
	* if not we check if the FromNode is a root and we had the CurrentNode has entry node
	* because the _Root is only cosmetic
	*/
	void CheckEntryNodeIsSet(UNpcBehaviorGraphNode* FromNode, UNpcBehaviorGraphNode* CurrentNode);
 
	/**
	* Remove all existing nodes
	*/
	virtual void RemoveAllNodes();

	/**
	* Set current blueprint edited to Task object inside a node
	*/
	void SetCurrentNpcBlueprintToNode(UNpcBehaviorGraphNode* Node);

	/**
	 * Find the node in the Nodes array in the graph by the task object given
	 */
	UNpcBehaviorGraphNode* FindNodeByTask(UNpcBehaviorTask_Default* Task);

};
