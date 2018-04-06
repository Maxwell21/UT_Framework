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
#include "StateMachineGraphNode_Entry.h"
#include "StateMachineGraphNode_Transition.h"
#include "StateMachineState_Default.h"
#include "StateMachineState_Entry.h"
#include "StateMachineTransition_Default.h"
#include "StateMachineBlueprint.h"
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
		UStateMachineState_Default* SmState = StateNode->State;
		FState& State = SmState->RuntimeData;

		// Generate state name
		State.Name = "NewState_" + FString::FromInt(SmState->GetUniqueID());
		
		// Create graph
		SmState->StateGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, *State.Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
		SmState->StateGraph->bAllowDeletion = false;
		SmState->StateGraph->bAllowRenaming = false;
		
		// Add to the blueprint graphs
		FBlueprintEditorUtils::AddUbergraphPage(Blueprint, SmState->StateGraph);
		
		State.BeginFunctionName = State.Name + SUFFIX_BEGIN_NAME;
		State.UpdateFunctionName = State.Name + SUFFIX_UPDATE_NAME;
		State.FinishFunctionName = State.Name + SUFFIX_FINISH_NAME;

		// Generate new events
		UFunction* BeginFunction = FindField<UFunction>(Blueprint->GetStateMachine()->GetClass(), "BeginState");
		UK2Node_CustomEvent* BeginEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 0), SmState->StateGraph, State.BeginFunctionName, BeginFunction, false);
		BeginEvent->bCanRenameNode = false;

		UFunction* UpdateFunction = FindField<UFunction>(Blueprint->GetStateMachine()->GetClass(), "UpdateState");
		UK2Node_CustomEvent* UpdateEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 100.f), SmState->StateGraph, State.UpdateFunctionName, UpdateFunction, false);
	
		TSharedPtr<FUserPinInfo> PinDefinition = MakeShareable(new FUserPinInfo);
		FEdGraphPinType PinType;
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		PinType.bIsReference = false;
		PinType.PinCategory = K2Schema->PC_Float;
		PinDefinition->PinName = "DeltaTime";
		PinDefinition->PinType = PinType;
		UEdGraphPin* NewPin = UpdateEvent->CreatePinFromUserDefinition(PinDefinition);
		UpdateEvent->ReconstructNode();
		UpdateEvent->bCanRenameNode = false;

		UFunction* FinishFunction = FindField<UFunction>(Blueprint->GetStateMachine()->GetClass(), "FinishState");
		UK2Node_CustomEvent* FinishEvent = UK2Node_CustomEvent::CreateFromFunction(FVector2D(0, 250.f), SmState->StateGraph, State.FinishFunctionName, FinishFunction, false);
		FinishEvent->bCanRenameNode = false;

		Blueprint->GetStateMachine()->AddOrUpdateState(State);
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
		FTransition& RuntimeData = Transition->RuntimeData;

		if (InputState && OutputState)
		{
			// Generate transition name
			RuntimeData.Name = "Transition_" + InputState->State->RuntimeData.Name + "To" + OutputState->State->RuntimeData.Name;
			RuntimeData.FromState = InputState->State->RuntimeData.Name;
			RuntimeData.ToState = OutputState->State->RuntimeData.Name;

			// Create function graph
			Transition->TransitionGraph = FBlueprintEditorUtils::CreateNewGraph(Blueprint, *RuntimeData.Name, UEdGraph::StaticClass(), UEdGraphSchema_K2::StaticClass());
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
						FName NewPinName = ResultNode->CreateUniquePinName(TEXT("Result"));
						UEdGraphPin* NewPin = ResultNode->CreateUserDefinedPin(NewPinName, PinType, EGPD_Input, false);
					
						ResultNode->ReconstructNode();
					}
				}
			}

			Blueprint->GetStateMachine()->AddOrUpdateTransition(RuntimeData);
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
			FState& RuntimeState = State->RuntimeData;
			RuntimeState.Name = NewName;

			FBlueprintEditorUtils::RenameGraph(State->StateGraph, RuntimeState.Name);
			
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

			RuntimeState.BeginFunctionName  = RuntimeState.Name + SUFFIX_BEGIN_NAME;
			RuntimeState.UpdateFunctionName = RuntimeState.Name + SUFFIX_UPDATE_NAME;
			RuntimeState.FinishFunctionName = RuntimeState.Name + SUFFIX_FINISH_NAME;

			State->EditedStateMachineBlueprint->GetStateMachine()->AddOrUpdateState(RuntimeState, OldName);
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
			FTransition& RuntimeData = Transition->RuntimeData;

			// check if the transition node is linked to through input pin and output pin
			if ((!TransNode->GetInputPin() || TransNode->GetInputPin()->LinkedTo.Num() == 0)
				|| (!TransNode->GetOutputPin() && TransNode->GetOutputPin()->LinkedTo.Num() == 0))
				return;

			UStateMachineGraphNode* InputState = CastChecked<UStateMachineGraphNode>(TransNode->GetInputPin()->LinkedTo[0]->GetOwningNode());
			UStateMachineGraphNode* OutputState = CastChecked<UStateMachineGraphNode>(TransNode->GetOutputPin()->LinkedTo[0]->GetOwningNode());

			FString OldName = Transition->TransitionGraph->GetName();

			// Generate new transition name
			RuntimeData.Name = "Transition_" + InputState->State->RuntimeData.Name + "To" + OutputState->State->RuntimeData.Name;
			RuntimeData.FromState = InputState->State->RuntimeData.Name;
			RuntimeData.ToState = OutputState->State->RuntimeData.Name;

			// Rename
			FBlueprintEditorUtils::RenameGraph(Transition->TransitionGraph, RuntimeData.Name);
			Transition->EditedStateMachineBlueprint->GetStateMachine()->AddOrUpdateTransition(RuntimeData, OldName);
		}
	};

};
