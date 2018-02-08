/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "StateMachineGraph.generated.h"

class UStateMachineBlueprint;
class UStateMachineState_Default;
class UStateMachineState_Entry;
class UStateMachineTransition_Default;
class UStateMachineGraphNode;
class UStateMachineGraphNode_Transition;

/**
 *
 */
UCLASS()
class UT_FRAMEWORKEDITOR_API UStateMachineGraph : public UEdGraph
{
	GENERATED_BODY()

public:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	UStateMachineBlueprint* StateMachineBlueprint;

protected:

	UStateMachineGraphNode* EntryNode;

public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	/**
	* Init properties for the graph and rebuild the graph
	*/
	void Init(UStateMachineBlueprint* InStateMachineBlueprint);

	/**
	* Completely rebuild the graph from the root, removing all old nodes
	*/
	void RebuildGraph();

	/**
	* Construct all nodes after loading the graph
	*/
 	void ReconstructNodes();

	/**
	* Register a new node to the graph and the state
	*/
	void RegisterNode(UStateMachineState_Default* State, UStateMachineGraphNode* NewNode, UEdGraphPin* FromPin);

	/**
	* Register a new entry node
	*/
	void RegisterEntryNode(UStateMachineState_Entry* Entry, UStateMachineGraphNode* NewNode);

	/**
	* Register a new transition node to the graph and the transition object
	*/
	void RegisterTransitionNode(UStateMachineTransition_Default* Transition, UStateMachineGraphNode_Transition* NewNode);

	/**
	* Use this graph to re-link all of the states it represents after a change in linkage
	*/
	void LinkStates();

	/**
	* Create the "Entry" node for begin the state machine (purely cosmetic)
	*/
	void InitDefaultNode();

	/**
	* Notify all nodes that graph has changed
	*/
	void NotifyNodesGraphChanged();

	/**
	* Get the right outer for creating the new State
	*/
	UObject* PrepareOuter();
	
	/**
	* Set the entry state to the entry node and the entry object
	*/
	void SetEntryState(UStateMachineState_Default* State);

protected:

	/**
	* Remove all existing nodes
	*/
	virtual void RemoveAllNodes();

	/**
	* Set current blueprint edited to State object inside a node
	*/
	void SetCurrentStateMachineBlueprintToNode(UStateMachineGraphNode* Node);

public:

	/**
	 * Find the node in the Nodes array in the graph by the state object given
	 */
	UStateMachineGraphNode* FindNodeByState(UStateMachineState_Default* State);

	/**
	* Find the node in the Nodes array in the graph by the transition object given
	*/
	UStateMachineGraphNode_Transition* FindNodeByTransition(UStateMachineTransition_Default* Transition);
};
