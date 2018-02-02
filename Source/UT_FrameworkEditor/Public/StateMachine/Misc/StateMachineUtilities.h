/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "BlueprintEditorUtils.h"
#include "StateMachineGraph.h"
#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Transition.h"
#include "StateMachineState_Default.h"
#include "StateMachineState_Entry.h"
#include "StateMachineTransition_Default.h"
#include "EditorUtilities.h"

class UT_FRAMEWORKEDITOR_API FStateMachineUtilities : public FEditorUtilities
{

	/************************************************************************/
	/* PROPERTIES															*/
	/************************************************************************/

protected:

	static const FString BEGIN_NAME;
	static const FString UPDATE_NAME;
	static const FString FINISH_NAME;
	static const FString SUFFIX_BEGIN_NAME;
	static const FString SUFFIX_UPDATE_NAME;
	static const FString SUFFIX_FINISH_NAME;
	
public:

	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/
	
	/**
	* Add new State
	*
	* @param	NodePosX				X coordinate to place node at
	* @param	NodePosY				Y coordinate to place node at
	* @param	UStateMachineGraph		Owner
	* @param	UEdGraphPin				FromPin
	* @param	bSelectNewNode			Whether or not to select the new node being created
	*
	* @return	Either a new node or an existing node representing the class
	*/
	template<typename NodeType, typename StateType>
	static NodeType* AddNewState(int32 NodePosX, int32 NodePosY, UStateMachineGraph* Graph, UEdGraphPin* FromPin = nullptr, bool bSelectNewNode = true)
	{
		// Create the new node
		NodeType* NewNode = CreateNode<NodeType>(Graph, NodePosX, NodePosY, bSelectNewNode);
		 
		// Create the object to edit
		StateType* State = NewObject<StateType>(Graph->PrepareOuter());
		 
		// Register node
		Graph->RegisterNode(State, NewNode, FromPin);
	
		return NewNode;
	};

	/**
	* Add new Entry
	*
	* @param	NodePosX				X coordinate to place node at
	* @param	NodePosY				Y coordinate to place node at
	* @param	UStateMachineGraph		Owner
	* @param	bSelectNewNode			Whether or not to select the new node being created
	*
	* @return	Either a new node or an existing node representing the class
	*/
	static UStateMachineGraphNode_Entry* AddNewEntry(int32 NodePosX, int32 NodePosY, UStateMachineGraph* Graph, bool bSelectNewNode = true)
	{
		// Create the new node
		UStateMachineGraphNode_Entry* NewNode = CreateNode<UStateMachineGraphNode_Entry>(Graph, NodePosX, NodePosY, bSelectNewNode);

		// Create the object to edit
		UStateMachineState_Entry* Entry = NewObject<UStateMachineState_Entry>(Graph->PrepareOuter());

		// Register node
		Graph->RegisterEntryNode(Entry, NewNode);

		return NewNode;
	};

	/**
	* Add new Transition
	*
	* @param	NodePosX				X coordinate to place node at
	* @param	NodePosY				Y coordinate to place node at
	* @param	UStateMachineGraph		Owner
	* @param	bSelectNewNode			Whether or not to select the new node being created
	*
	* @return	Either a new node or an existing node representing the class
	*/
	template<typename NodeType, typename TransitionType>
	static NodeType* AddNewTransition(int32 NodePosX, int32 NodePosY, UStateMachineGraph* Graph, bool bSelectNewNode = true)
	{
		// Create the new node
		NodeType* NewNode = CreateNode<NodeType>(Graph, NodePosX, NodePosY, bSelectNewNode);

		// Create the object to edit
		TransitionType* Transition = NewObject<TransitionType>(Graph->PrepareOuter());

		// Register node
		Graph->RegisterTransitionNode(Transition, NewNode);

		return NewNode;
	};

	/**
	 * Create a new graph an bound it to the state
	 *
	 * @param UStateMachineGraph* Graph
	 * @param UStateMachineGraphNode* StateNode
	 */
	static void AddNewBoundedGraphToState(UStateMachineGraph* Graph, UStateMachineGraphNode* StateNode)
	{
		// it's the root node, skip it
		if (StateNode->GetRootNode())
			return;

		UStateMachineBlueprint* Blueprint = Graph->StateMachineBlueprint;
		UStateMachineState_Default* State = StateNode->State;

		// Generate state name
		State->StateName = "NewState_" + FString::FromInt(State->GetUniqueID());
		
		// Create graph
		State->StateGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, *State->StateName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
		State->StateGraph->bAllowDeletion = false;
		State->StateGraph->bAllowRenaming = false;
		
		// Add to the blueprint graphs
		FBlueprintEditorUtils::AddUbergraphPage(Blueprint, State->StateGraph);

		// Generate new events
		UFunction* BeginFunction = FindField<UFunction>(State->GetClass(), "BeginState");
		UK2Node_CustomEvent* BeginEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 0), State->StateGraph, State->StateName + SUFFIX_BEGIN_NAME, BeginFunction, false);
		BeginEvent->bCanRenameNode = false;

		UFunction* UpdateFunction = FindField<UFunction>(State->GetClass(), "UpdateState");
		UK2Node_CustomEvent* UpdateEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 100.f), State->StateGraph, State->StateName + SUFFIX_UPDATE_NAME, UpdateFunction, false);
		UpdateEvent->bCanRenameNode = false;

		UFunction* FinishFunction = FindField<UFunction>(State->GetClass(), "FinishState");
		UK2Node_CustomEvent* FinishEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 250.f), State->StateGraph, State->StateName + SUFFIX_FINISH_NAME, FinishFunction, false);
		FinishEvent->bCanRenameNode = false;

		State->BeginStateFunctionName = State->StateName + SUFFIX_BEGIN_NAME;
		State->UpdateStateFunctionName = State->StateName + SUFFIX_UPDATE_NAME;
		State->FinishStateFunctionName = State->StateName + SUFFIX_FINISH_NAME;

		
	};

	/**
	* Create a new graph an bound it to the transition
	*
	* @param UStateMachineGraph* Graph
	* @param UStateMachineGraphNode_Transition* TransNode
	*/
	static void AddNewBoundedGraphToTransition(UStateMachineGraph* Graph, UStateMachineGraphNode_Transition* TransNode)
	{
		UStateMachineBlueprint* Blueprint = Graph->StateMachineBlueprint;
		// check if the transition node is linked to through input pin and output pin
		if ((!TransNode->GetInputPin() || TransNode->GetInputPin()->LinkedTo.Num() == 0)
			|| (!TransNode->GetOutputPin() && TransNode->GetOutputPin()->LinkedTo.Num() == 0))
			return;
		
		UStateMachineGraphNode* InputState = CastChecked<UStateMachineGraphNode>(TransNode->GetInputPin()->LinkedTo[0]->GetOwningNode());
		UStateMachineGraphNode* OutputState = CastChecked<UStateMachineGraphNode>(TransNode->GetOutputPin()->LinkedTo[0]->GetOwningNode());
		UStateMachineTransition_Default* Transition = TransNode->Transition;

		if (InputState && OutputState)
		{
			// Generate transition name
			Transition->TransitionName = "Transition_" + InputState->State->StateName + "To" + OutputState->State->StateName;

			// Create function graph
			Transition->TransitionGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, *Transition->TransitionName, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
			Transition->TransitionGraph->bAllowDeletion = false;
			Transition->TransitionGraph->bAllowRenaming = false;
			FBlueprintEditorUtils::AddFunctionGraph<UClass>(Blueprint, Transition->TransitionGraph, true, nullptr);
			
			UK2Node_FunctionEntry* EntryNode = nullptr;
			if (Transition->TransitionGraph)
			{
				TArray<UK2Node_FunctionEntry*> EntryNodes;
				Transition->TransitionGraph->GetNodesOfClass(EntryNodes);

				if ((EntryNodes.Num() > 0) && EntryNodes[0]->IsEditable())
					EntryNode = EntryNodes[0];

				// create result node
				if (EntryNode)
				{
					UK2Node_FunctionResult* ResultNode = FBlueprintEditorUtils::FindOrCreateFunctionResultNode(EntryNode);
					if (ResultNode)
					{
						const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

						FEdGraphPinType PinType;
						PinType.bIsReference = false;
						PinType.PinCategory = K2Schema->PC_Boolean;
						const FString NewPinName = ResultNode->CreateUniquePinName(TEXT("Result"));
						UEdGraphPin* NewPin = ResultNode->CreateUserDefinedPin(NewPinName, PinType, EGPD_Input, false);
					
						ResultNode->ReconstructNode();
					}
				}
			}
		}
	};

	/**
	 * Rename the bounded graph to the state 
	 *
	 * @param UStateMachineState_Default* State
	 * @param FString OldName
	 * @param FString NewName
	 */
	static void RenameBoundingGraph(UStateMachineState_Default* State, FString OldName, FString NewName)
	{
		if (State && State->StateGraph)
		{
			State->StateName = NewName;

			FBlueprintEditorUtils::RenameGraph(State->StateGraph, State->StateName);
			
			for (UEdGraphNode* Node : State->StateGraph->Nodes)
			{
				FString NodeName = Node->GetNodeTitle(ENodeTitleType::MenuTitle).ToString();
				if (NodeName == OldName + SUFFIX_BEGIN_NAME)
					Node->OnRenameNode(NewName + SUFFIX_BEGIN_NAME);
				else if (NodeName == OldName + SUFFIX_UPDATE_NAME)
					Node->OnRenameNode(NewName + SUFFIX_UPDATE_NAME);
				else if (NodeName == OldName + SUFFIX_FINISH_NAME)
					Node->OnRenameNode(NewName + SUFFIX_FINISH_NAME);
			}

			State->BeginStateFunctionName  = State->StateName + SUFFIX_BEGIN_NAME;
			State->UpdateStateFunctionName = State->StateName + SUFFIX_UPDATE_NAME;
			State->FinishStateFunctionName = State->StateName + SUFFIX_FINISH_NAME;
		}
	};

	/**
	* Rename the bounded graph to the transition
	*
	* @param UStateMachineTransition_Default* Transition
	*/
	static void RenameBoundingGraph(UStateMachineTransition_Default* Transition)
	{
		if (Transition && Transition->TransitionGraph)
		{
			UStateMachineGraphNode_Transition* TransNode = CastChecked<UStateMachineGraphNode_Transition>(Transition->GraphNode);

			// check if the transition node is linked to through input pin and output pin
			if ((!TransNode->GetInputPin() || TransNode->GetInputPin()->LinkedTo.Num() == 0)
				|| (!TransNode->GetOutputPin() && TransNode->GetOutputPin()->LinkedTo.Num() == 0))
				return;

			UStateMachineGraphNode* InputState = CastChecked<UStateMachineGraphNode>(TransNode->GetInputPin()->LinkedTo[0]->GetOwningNode());
			UStateMachineGraphNode* OutputState = CastChecked<UStateMachineGraphNode>(TransNode->GetOutputPin()->LinkedTo[0]->GetOwningNode());

			// Generate new transition name
			FString TransitionName = "Transition_" + InputState->State->StateName + "To" + OutputState->State->StateName;

			// Rename
			Transition->TransitionName = TransitionName;
			FBlueprintEditorUtils::RenameGraph(Transition->TransitionGraph, Transition->TransitionName);
		}
	};

};
