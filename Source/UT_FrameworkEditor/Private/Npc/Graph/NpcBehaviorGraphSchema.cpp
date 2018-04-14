/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "NpcBehaviorGraphSchema.h"
#include "NpcBehaviorGraph.h"
#include "NpcBehaviorTaskFactory.h"
#include "NpcBehaviorTask_Wait.h"
#include "NpcBehaviorGraphNode_Wait.h"
#include "NpcBehaviorUtilities.h"
#include "EdGraph/EdGraph.h"
#include "GraphEditorActions.h"
#include "ScopedTransaction.h"
#include "MultiBoxBuilder.h"
#include "GenericCommands.h"
#include "NpcBehaviorTaskFactory_Wait.h"

#define LOCTEXT_NAMESPACE "NpcBehaviorGraphSchema"

/************************************************************************/
/* FNpcBehaviorGraphSchemaAction_NewTask                                */
/************************************************************************/

UEdGraphNode* FNpcBehaviorGraphSchemaAction_NewTask::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode/* = true*/)
{
	TSharedPtr<FNpcBehaviorTaskFactory> Factory = FNpcBehaviorUtilities::GetRegisteredTask(TaskName);
	if (Factory.IsValid())
	{
		if (UEdGraphNode* Node = Factory->Execute(Cast<UNpcBehaviorGraph>(ParentGraph), FromPin))
		{
			Node->NodePosX = Location.X;
			Node->NodePosY = Location.Y;

			return Node;
		}
	}
		
	return nullptr;
}

/************************************************************************/
/* UNpcBehaviorGraphSchema				                                */
/************************************************************************/

UNpcBehaviorGraphSchema::UNpcBehaviorGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UNpcBehaviorGraphSchema::ConnectionCausesLoop(const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	UNpcBehaviorGraphNode* InputNode = Cast<UNpcBehaviorGraphNode>(InputPin->GetOwningNode());
	UNpcBehaviorGraphNode* OutputNode = Cast<UNpcBehaviorGraphNode>(OutputPin->GetOwningNode());

	return InputNode == OutputNode;
}

void UNpcBehaviorGraphSchema::GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

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

		MenuBuilder.AddMenuEntry(Description, Description, FSlateIcon(), FUIAction(
		FExecuteAction::CreateUObject((UNpcBehaviorGraphSchema*const)this, &UNpcBehaviorGraphSchema::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

void UNpcBehaviorGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	for (auto const& TaskFactory : FNpcBehaviorUtilities::GetAllRegisteredTasks())
	{
		TSharedPtr<FNpcBehaviorGraphSchemaAction_NewTask> NewTaskAction(new FNpcBehaviorGraphSchemaAction_NewTask(
			FText::GetEmpty(),
			TaskFactory->GetNodeName(),
			TaskFactory->GetNodeTooltip(),
			0,
			TaskFactory->GetName()
		));
	
		ContextMenuBuilder.AddAction(NewTaskAction);
	}
}
 
void UNpcBehaviorGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if (InGraphPin)
	{
		MenuBuilder->BeginSection("NpcBehaviorGraphSchemaPinActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		{
			// Only display the 'Break Links' option if there is a link to break!
			if (InGraphPin->LinkedTo.Num() > 0)
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);

				// add sub menu for break link to
				if (InGraphPin->LinkedTo.Num() > 1)
				{
					MenuBuilder->AddSubMenu(
						LOCTEXT("BreakLinkTo", "Break Link To..."),
						LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
						FNewMenuDelegate::CreateUObject((UNpcBehaviorGraphSchema*const)this, &UNpcBehaviorGraphSchema::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));
					MenuBuilder->AddSubMenu(
						LOCTEXT("StraightenConnection", "Straighten Connection To..."),
						LOCTEXT("StraightenConnection_Tip", "Straighten a specific connection"),
						FNewMenuDelegate::CreateUObject(this, &UEdGraphSchema_K2::GetStraightenConnectionToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));
				}
				else
				{
					((UNpcBehaviorGraphSchema*const)this)->GetBreakLinkToSubMenuActions(*MenuBuilder, const_cast<UEdGraphPin*>(InGraphPin));

					UEdGraphPin* Pin = InGraphPin->LinkedTo[0];
					FText PinName = Pin->GetDisplayName();
					FText NodeName = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView);

					MenuBuilder->AddMenuEntry(
						FGraphEditorCommands::Get().StraightenConnections,
						NAME_None,
						FText::Format(LOCTEXT("StraightenDescription_SinglePin", "Straighten Connection to {0}"), NodeName),
						FText::Format(LOCTEXT("StraightenDescription_SinglePin_Node_Tip", "Straighten the connection between this pin, and {0}"), NodeName),
						FSlateIcon(NAME_None, NAME_None, NAME_None)
					);
				}
			}
		}

		MenuBuilder->EndSection();
	}
}

const FPinConnectionResponse UNpcBehaviorGraphSchema::CanCreateConnection(const UEdGraphPin* PinA, const UEdGraphPin* PinB) const
{
	// Make sure the pins are not on the same node
	if (PinA->GetOwningNode() == PinB->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionSameNode", "Both are on the same node"));
	}

	// Compare the directions
	const UEdGraphPin* InputPin = NULL;
	const UEdGraphPin* OutputPin = NULL;

	if (!CategorizePinsByDirection(PinA, PinB, /*out*/ InputPin, /*out*/ OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionIncompatible", "Directions are not compatible"));
	}

	if (ConnectionCausesLoop(InputPin, OutputPin))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("ConnectionLoop", "Connection would cause loop"));
	}

	// Break existing connections on outputs only - multiple output connections are acceptable
	if (OutputPin->LinkedTo.Num() > 0)
	{
		ECanCreateConnectionResponse ReplyBreakOutputs;
		if (OutputPin == PinA)
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_A;
		}
		else
		{
			ReplyBreakOutputs = CONNECT_RESPONSE_BREAK_OTHERS_B;
		}
		return FPinConnectionResponse(ReplyBreakOutputs, LOCTEXT("ConnectionReplace", "Replace existing connections"));
	}

	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

bool UNpcBehaviorGraphSchema::TryCreateConnection(UEdGraphPin* PinA, UEdGraphPin* PinB) const
{
	if (UEdGraphSchema::TryCreateConnection(PinA, PinB))
	{
		CastChecked<UNpcBehaviorGraph>(PinA->GetOwningNode()->GetGraph())->SetNextTask(PinA, Cast<UNpcBehaviorGraphNode>(PinB->GetOwningNode()));

		return true;
	}

	return false;
}

bool UNpcBehaviorGraphSchema::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	return false;
}

FLinearColor UNpcBehaviorGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

void UNpcBehaviorGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	UEdGraphSchema::BreakNodeLinks(TargetNode);
}

void UNpcBehaviorGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	UNpcBehaviorGraphNode* Node = Cast<UNpcBehaviorGraphNode>(TargetPin.GetOwningNode());
	if (Node && TargetPin.Direction == EGPD_Output)
		Node->ResetLinkedTask(&TargetPin);
	else
	{
		for (UEdGraphPin* CurrentPin : TargetPin.LinkedTo)
		{
			if (UNpcBehaviorGraphNode* OwningNode = Cast<UNpcBehaviorGraphNode>(CurrentPin->GetOwningNode()))
				OwningNode->ResetLinkedTask(CurrentPin);
		}
	}

	UEdGraphSchema::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UNpcBehaviorGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));
	
	if (SourcePin->Direction == EGPD_Output)
		Cast<UNpcBehaviorGraphNode>(SourcePin->GetOwningNode())->ResetLinkedTask(SourcePin);
	else
	{
		for (UEdGraphPin* CurrentPin : SourcePin->LinkedTo)
		{
			if (UNpcBehaviorGraphNode* OwningNode = Cast<UNpcBehaviorGraphNode>(CurrentPin->GetOwningNode()))
				OwningNode->ResetLinkedTask(CurrentPin);
		}
	}

	UEdGraphSchema::BreakSinglePinLink(SourcePin, TargetPin);
}

#undef LOCTEXT_NAMESPACE
