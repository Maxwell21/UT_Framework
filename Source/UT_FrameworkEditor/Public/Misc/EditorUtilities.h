/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraphPin.h"

class UT_FRAMEWORKEDITOR_API FEditorUtilities
{

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

public:

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
};
