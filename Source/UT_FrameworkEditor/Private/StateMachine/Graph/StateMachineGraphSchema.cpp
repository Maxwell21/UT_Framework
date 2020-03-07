/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "StateMachineGraphSchema.h"
#include "StateMachineGraph.h"
#include "StateMachineGraphNode.h"
#include "StateMachineGraphNode_Entry.h"
#include "StateMachineGraphNode_Transition.h"
#include "StateMachineTransition_Default.h"
#include "StateMachineUtilities.h"
#include "BlueprintEditorUtils.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "MultiBoxBuilder.h"
#include "GenericCommands.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "ToolMenus/Public/ToolMenus.h"
#include "ToolMenus/Public/ToolMenu.h"
#include "ToolMenus/Public/ToolMenuSection.h"

#define LOCTEXT_NAMESPACE "StateMachineGraphSchema"

/************************************************************************/
/* FStateMachineGraphSchemaAction_NewState                               */
/************************************************************************/

UEdGraphNode* FStateMachineGraphSchemaAction_NewState::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	if (UEdGraphNode* Node = FStateMachineUtilities::AddNewState<UStateMachineGraphNode, UStateMachineState_Default>(Location.X, Location.Y, Cast<UStateMachineGraph>(ParentGraph), FromPin, bSelectNewNode))
		return Node;

	return nullptr;
}

/************************************************************************/
/* UStateMachineGraphSchema				                                */
/************************************************************************/

UStateMachineGraphSchema::UStateMachineGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UStateMachineGraphSchema::ConnectionCausesLoop(const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	UStateMachineGraphNode* InputNode = Cast<UStateMachineGraphNode>(InputPin->GetOwningNode());
	UStateMachineGraphNode* OutputNode = Cast<UStateMachineGraphNode>(OutputPin->GetOwningNode());

	return InputNode == OutputNode;
}

void UStateMachineGraphSchema::GetBreakLinkToSubMenuActions(class UToolMenu* Menu, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

	FToolMenuSection& Section = Menu->FindOrAddSection("AIGraphSchemaPinActions");

	// Add all the links we could break from
	for (TArray<class UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName.ToString());

			// Add name of connection if possible
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32 &Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		++Count;

		Section.AddMenuEntry(NAME_None, Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject((UStateMachineGraphSchema*const)this, &UStateMachineGraphSchema::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

void UStateMachineGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FStateMachineGraphSchemaAction_NewState> NewStateAction(new FStateMachineGraphSchemaAction_NewState(
		FText::GetEmpty(),
		LOCTEXT("NewState", "New State"),
		LOCTEXT("NewStateTooltip", "Add a new State"),
		0
	));
	
	ContextMenuBuilder.AddAction(NewStateAction);
}

void UStateMachineGraphSchema::GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	if (Context->Pin)
	{
		FToolMenuSection& Section = Menu->AddSection("StateMachineGraphSchemaPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		// Only display the 'Break Links' option if there is a link to break!
		if (Context->Pin->LinkedTo.Num() > 0)
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
			Section.AddMenuEntry(FGraphEditorCommands::Get().StraightenConnections);

			// add sub menu for break link to
			if (Context->Pin->LinkedTo.Num() > 1)
			{
				Section.AddSubMenu(
					"BreakLinkTo",
					LOCTEXT("BreakLinkTo", "Break Link To..."),
					LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
					FNewToolMenuDelegate::CreateUObject((UStateMachineGraphSchema* const)this, &UStateMachineGraphSchema::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(Context->Pin)));
				Section.AddSubMenu(
					"StraightenConnection",
					LOCTEXT("StraightenConnection", "Straighten Connection To..."),
					LOCTEXT("StraightenConnection_Tip", "Straighten a specific connection"),
					FNewToolMenuDelegate::CreateUObject(this, &UEdGraphSchema_K2::GetStraightenConnectionToSubMenuActions, const_cast<UEdGraphPin*>(Context->Pin)));
			}
			else
			{
				((UStateMachineGraphSchema* const)this)->GetBreakLinkToSubMenuActions(Menu, const_cast<UEdGraphPin*>(Context->Pin));

			}
		}
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UStateMachineGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are on the same node"));
	}

	// Connect entry node to a state is OK
	const bool bPinAIsEntry = PinA->GetOwningNode()->IsA(UStateMachineGraphNode_Entry::StaticClass());
	const bool bPinBIsEntry = PinB->GetOwningNode()->IsA(UStateMachineGraphNode_Entry::StaticClass());
	const bool bPinAIsStateNode = PinA->GetOwningNode()->IsA(UStateMachineGraphNode::StaticClass());
	const bool bPinBIsStateNode = PinB->GetOwningNode()->IsA(UStateMachineGraphNode::StaticClass());

	if (bPinAIsEntry || bPinBIsEntry)
	{
		if (bPinAIsEntry && bPinBIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}

		if (bPinBIsEntry && bPinAIsStateNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
		}

		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Entry must connect to a state node"));
	}


	const bool bPinAIsTransition = PinA->GetOwningNode()->IsA(UStateMachineGraphNode_Transition::StaticClass());
	const bool bPinBIsTransition = PinB->GetOwningNode()->IsA(UStateMachineGraphNode_Transition::StaticClass());

	if (bPinAIsTransition && bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot wire a transition to a transition"));
	}

	// Compare the directions
	bool bDirectionsOK = false;

	if ((PinA->Direction == EGPD_Input) && (PinB->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}
	else if ((PinB->Direction == EGPD_Input) && (PinA->Direction == EGPD_Output))
	{
		bDirectionsOK = true;
	}

	/*
	if (!bDirectionsOK)
	{
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Directions are not compatible"));
	}
	*/

	// Transitions are exclusive (both input and output), but states are not
	if (bPinAIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
	}
	else if (bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT(""));
	}
	else if (!bPinAIsTransition && !bPinBIsTransition)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Create a transition"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
	}
}

bool UStateMachineGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	if (PinB->Direction == PinA->Direction)
	{
		if (UStateMachineGraphNode* Node = Cast<UStateMachineGraphNode>(PinB->GetOwningNode()))
		{
			if (PinA->Direction == EGPD_Input)
				PinB = Node->GetOutputPin();
			else
				PinB = Node->GetInputPin();
		}
	}

	UStateMachineGraphNode_Entry* EntryNode = Cast<UStateMachineGraphNode_Entry>(PinA->GetOwningNode());
	UStateMachineGraphNode* Node = Cast<UStateMachineGraphNode>(PinB->GetOwningNode());
	if (EntryNode && Node)
	{
		if (UStateMachineGraph* Graph = Cast<UStateMachineGraph>(EntryNode->GetGraph()))
			Graph->SetEntryState(Node->State);
	}

	const bool bModified = UEdGraphSchema::TryCreateConnection(PinA, PinB);

	if (bModified)
	{
		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(PinA->GetOwningNode());
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	}

	return bModified;
}

bool UStateMachineGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	UStateMachineGraphNode* NodeA = Cast<UStateMachineGraphNode>(PinA->GetOwningNode());
	UStateMachineGraphNode* NodeB = Cast<UStateMachineGraphNode>(PinB->GetOwningNode());

	if ((NodeA != nullptr) && (NodeB != nullptr)
		&& (NodeA->GetInputPin() != nullptr) && (NodeA->GetOutputPin() != nullptr)
		&& (NodeB->GetInputPin() != nullptr) && (NodeB->GetOutputPin() != nullptr))
	{
		UStateMachineGraphNode_Transition* TransitionNode = FStateMachineUtilities::AddNewTransition<UStateMachineGraphNode_Transition, UStateMachineTransition_Default>(0, 0, Cast<UStateMachineGraph>(NodeA->GetGraph()), false);

		if (PinA->Direction == EGPD_Output)
			TransitionNode->CreateConnections(NodeA, NodeB);
		else
			TransitionNode->CreateConnections(NodeB, NodeA);

		UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(TransitionNode->GetGraph());
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);

		return true;
	}

	return false;
}

bool UStateMachineGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return false;
}

FLinearColor UStateMachineGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UStateMachineGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	UEdGraphSchema::BreakNodeLinks(TargetNode);
}

void UStateMachineGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	UStateMachineGraphNode* Node = Cast<UStateMachineGraphNode>(TargetPin.GetOwningNode());
// 	if (Node && TargetPin.Direction == EGPD_Output)
// 		Node->ResetLinkedState(&TargetPin);
// 	else
// 	{
// 		for (UEdGraphPin* CurrentPin : TargetPin.LinkedTo)
// 		{
// 			if (UStateMachineGraphNode* OwningNode = Cast<UStateMachineGraphNode>(CurrentPin->GetOwningNode()))
// 				OwningNode->ResetLinkedState(CurrentPin);
// 		}
// 	}

	UEdGraphSchema::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UStateMachineGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

// 	if (SourcePin->Direction == EGPD_Output)
// 		Cast<UStateMachineGraphNode>(SourcePin->GetOwningNode())->ResetLinkedState(SourcePin);
// 	else
// 	{
// 		for (UEdGraphPin* CurrentPin : SourcePin->LinkedTo)
// 		{
// 			if (UStateMachineGraphNode* OwningNode = Cast<UStateMachineGraphNode>(CurrentPin->GetOwningNode()))
// 				OwningNode->ResetLinkedState(CurrentPin);
// 		}
// 	}

	UEdGraphSchema::BreakSinglePinLink(SourcePin, TargetPin);
}

#undef LOCTEXT_NAMESPACE
