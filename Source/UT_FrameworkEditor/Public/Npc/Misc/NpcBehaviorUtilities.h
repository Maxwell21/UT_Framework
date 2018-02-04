/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraphPin.h"

class UNpcBehaviorGraph;
class FNpcBehaviorTaskFactory;

class UT_FRAMEWORKEDITOR_API FNpcBehaviorUtilities
{

protected:

	/************************************************************************/
	/* PROPERTIES                                                           */
	/************************************************************************/

	static TArray<TSharedPtr<FNpcBehaviorTaskFactory>> RegisteredTasks;
	
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

	/**
	* Add new Task
	*
	* @param	NodePosX				X coordinate to place node at
	* @param	NodePosY				Y coordinate to place node at
	* @param	UNpcBehaviorGraph		Owner
	* @param	UEdGraphPin				FromPin
	* @param	bSelectNewNode	Whether or not to select the new node being created
	*
	* @return	Either a new node or an existing node representing the class
	*/
	template<typename NodeType, typename TaskType>
	static NodeType* AddNewTask(int32 NodePosX, int32 NodePosY, UNpcBehaviorGraph* Graph, UEdGraphPin* FromPin = nullptr, bool bSelectNewNode = true)
	{
		// Create the new node
		NodeType* NewNode = CreateNode<NodeType>(Graph, NodePosX, NodePosY, bSelectNewNode);
		 
		// Create the object to edit
		TaskType* Task = NewObject<TaskType>(Graph->PrepareOuter());
		 
		// Register node
		Graph->RegisterNode(Task, NewNode, FromPin);
	
		return NewNode;
	};

	/**
	* Create a new node
	*
	* @param	Graph		Owner
	* @param	NodePosX	X coordinate to place node at
	* @param	NodePosY	Y coordinate to place node at
	* @param	bSelectNewNode	Whether or not to select the new node being created
	*
	* @return	Either a new node or an existing node representing the class
	*/
	template<typename T>
	static T* CreateNode(UEdGraph* Graph, int32 NodePosX, int32 NodePosY, bool bSelectNewNode = true)
	{
		FGraphNodeCreator<T> NodeCreator(*Graph);
		T* GraphNode = NodeCreator.CreateNode();
		GraphNode->NodePosX = NodePosX;
		GraphNode->NodePosY = NodePosY;
		NodeCreator.Finalize();

		return GraphNode;
	};

	/**
	 * Register a new task factory
	 *
	 * @param TSharedPtr<FNpcBehaviorTaskFactory> NodeFactory
	 */
	static void RegisterTask(TSharedPtr<FNpcBehaviorTaskFactory> NodeFactory);

	/**
	* Get a registered task factory
	*
	* @param FName TaskName
	*/
	static TSharedPtr<FNpcBehaviorTaskFactory> GetRegisteredTask(FName TaskName);

	/**
	* Get all registered tasks
	*/
	static TArray<TSharedPtr<FNpcBehaviorTaskFactory>> GetAllRegisteredTasks();
};
